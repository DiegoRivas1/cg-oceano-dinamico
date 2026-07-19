//
// Created by DIEGO on 13/07/2026.
//

#ifndef CG_OCEANO_DINAMICO_OBJLOADER_H
#define CG_OCEANO_DINAMICO_OBJLOADER_H
//#pragma once
// OBJLoader.h
// Parser minimo de Wavefront .obj para geometria estatica. Soporta:
//  - v / vt / vn globales (los indices de un .obj son GLOBALES al archivo,
//    no se reinician por grupo, asi que siempre se parsean todos)
//  - filtrar caras por nombre de grupo ("g NombreGrupo" u "o NombreObjeto"),
//    util cuando el archivo trae varios objetos mezclados (el Medieval
//    Boat, por ejemplo, incluye un "Plane" de fondo ademas del barco real
//    "MED-BOAT" sin filtrar cargariamos ambos)
//  - caras triangulares o poligonales (fan-triangulation si tiene >3 vertices)
//  - formatos de cara "v", "v/vt", "v//vn", "v/vt/vn"
//  - normaliza automaticamente la escala del modelo a un tamano deseado,
//    porque no sabemos de antemano en que unidades fue exportado (cm, m,
//    etc. segun la configuracion de Cinema4D/Blender/etc.)

#include <array>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct VerticeOBJ {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct MallaOBJ {
    std::vector<VerticeOBJ> vertices; // SIN indices: cada triangulo son 3 entradas nuevas
};

namespace OBJLoader {

namespace detalle {

inline VerticeOBJ parsearVertice(const std::string& token,
                                  const std::vector<std::array<float, 3>>& posiciones,
                                  const std::vector<std::array<float, 2>>& uvs,
                                  const std::vector<std::array<float, 3>>& normales) {
    int vi = 0, ti = 0, ni = 0;
    size_t p1 = token.find('/');
    if (p1 == std::string::npos) {
        vi = std::stoi(token);
    } else {
        vi = std::stoi(token.substr(0, p1));
        size_t p2 = token.find('/', p1 + 1);
        if (p2 == std::string::npos) {
            if (p1 + 1 < token.size()) ti = std::stoi(token.substr(p1 + 1));
        } else {
            if (p2 > p1 + 1) ti = std::stoi(token.substr(p1 + 1, p2 - p1 - 1));
            if (p2 + 1 < token.size()) ni = std::stoi(token.substr(p2 + 1));
        }
    }

    VerticeOBJ v{};
    if (vi > 0 && vi <= static_cast<int>(posiciones.size())) {
        const auto& p = posiciones[vi - 1];
        v.x = p[0]; v.y = p[1]; v.z = p[2];
    }
    if (ti > 0 && ti <= static_cast<int>(uvs.size())) {
        const auto& t = uvs[ti - 1];
        v.u = t[0]; v.v = t[1];
    }
    if (ni > 0 && ni <= static_cast<int>(normales.size())) {
        const auto& n = normales[ni - 1];
        v.nx = n[0]; v.ny = n[1]; v.nz = n[2];
    }
    return v;
}

} // namespace detalle

// grupoDeseado vacio ("") = cargar TODO el archivo, sin filtrar por grupo.
inline MallaOBJ cargar(const std::string& ruta, const std::string& grupoDeseado = "") {
    MallaOBJ malla;

    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "[OBJLoader] No se pudo abrir: " << ruta << "\n";
        return malla;
    }

    std::vector<std::array<float, 3>> posiciones;
    std::vector<std::array<float, 2>> uvs;
    std::vector<std::array<float, 3>> normales;

    std::string grupoActual;
    bool grupoCoincide = grupoDeseado.empty();
    bool archivoTraeNormales = false;

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::istringstream ss(linea);
        std::string tipo;
        ss >> tipo;

        if (tipo == "g" || tipo == "o") {
            ss >> grupoActual;
            grupoCoincide = grupoDeseado.empty() || (grupoActual == grupoDeseado);
        } else if (tipo == "v") {
            std::array<float, 3> p{};
            ss >> p[0] >> p[1] >> p[2];
            posiciones.push_back(p);
        } else if (tipo == "vt") {
            std::array<float, 2> t{};
            ss >> t[0] >> t[1];
            uvs.push_back(t);
        } else if (tipo == "vn") {
            std::array<float, 3> n{};
            ss >> n[0] >> n[1] >> n[2];
            normales.push_back(n);
            archivoTraeNormales = true;
        } else if (tipo == "f" && grupoCoincide) {
            std::vector<std::string> tokens;
            std::string tok;
            while (ss >> tok) tokens.push_back(tok);
            if (tokens.size() < 3) continue;

            std::vector<VerticeOBJ> poligono;
            poligono.reserve(tokens.size());
            for (const auto& t : tokens) poligono.push_back(detalle::parsearVertice(t, posiciones, uvs, normales));

            // Fan-triangulation: (0,1,2), (0,2,3), (0,3,4), ... - soporta triangulos y quads/n-gonos.
            for (size_t i = 1; i + 1 < poligono.size(); ++i) {
                malla.vertices.push_back(poligono[0]);
                malla.vertices.push_back(poligono[i]);
                malla.vertices.push_back(poligono[i + 1]);
            }
        }
    }

    std::cout << "[OBJLoader] " << ruta << " (grupo='" << grupoDeseado << "'): "
              << malla.vertices.size() / 3 << " triangulos cargados\n";
    if (malla.vertices.empty()) {
        std::cerr << "[OBJLoader] Advertencia: 0 triangulos. Revisa el nombre del grupo/objeto pedido.\n";
    }

    // Si el archivo no traia vn, calculamos normales planas por cara (flat shading).
    if (!archivoTraeNormales) {
        for (size_t i = 0; i + 2 < malla.vertices.size(); i += 3) {
            VerticeOBJ& a = malla.vertices[i];
            VerticeOBJ& b = malla.vertices[i + 1];
            VerticeOBJ& c = malla.vertices[i + 2];
            float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z;
            float vx = c.x - a.x, vy = c.y - a.y, vz = c.z - a.z;
            float nx = uy * vz - uz * vy;
            float ny = uz * vx - ux * vz;
            float nz = ux * vy - uy * vx;
            float len = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (len > 1e-6f) { nx /= len; ny /= len; nz /= len; }
            a.nx = b.nx = c.nx = nx;
            a.ny = b.ny = c.ny = ny;
            a.nz = b.nz = c.nz = nz;
        }
    }

    return malla;
}

// Centra el modelo en X/Z, apoya su base en y=0, y lo reescala para que su
// dimension mas grande mida 'tamanoObjetivo'. Necesario porque no conocemos
// de antemano en que unidades fue exportado el .obj original.
inline void normalizarEscala(MallaOBJ& malla, float tamanoObjetivo) {
    if (malla.vertices.empty()) return;

    float minX = 1e9f, minY = 1e9f, minZ = 1e9f;
    float maxX = -1e9f, maxY = -1e9f, maxZ = -1e9f;
    for (const auto& v : malla.vertices) {
        minX = std::min(minX, v.x); maxX = std::max(maxX, v.x);
        minY = std::min(minY, v.y); maxY = std::max(maxY, v.y);
        minZ = std::min(minZ, v.z); maxZ = std::max(maxZ, v.z);
    }

    float centroX = (minX + maxX) * 0.5f;
    float centroZ = (minZ + maxZ) * 0.5f;
    float dimMax = std::max({maxX - minX, maxY - minY, maxZ - minZ});
    if (dimMax < 1e-6f) return;

    float escala = tamanoObjetivo / dimMax;
    for (auto& v : malla.vertices) {
        v.x = (v.x - centroX) * escala;
        v.y = (v.y - minY) * escala; // apoya la base del modelo en y=0
        v.z = (v.z - centroZ) * escala;
    }
}

} // namespace OBJLoader
#endif //CG_OCEANO_DINAMICO_OBJLOADER_H