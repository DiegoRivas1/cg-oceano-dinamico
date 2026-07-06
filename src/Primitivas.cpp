#include "Primitivas.h"
#include <cmath>

namespace {
constexpr float PI = 3.14159265358979323846f;

void agregarVertice(MallaSimple& malla, float x, float y, float z, float nx, float ny, float nz) {
    malla.vertices.push_back({x, y, z, nx, ny, nz});
}
} // namespace

MallaSimple Primitivas::crearCaja(float ancho, float alto, float profundidad) {
    MallaSimple malla;
    float ax = ancho * 0.5f, ay = alto * 0.5f, az = profundidad * 0.5f;

    // 6 caras, 4 vertices cada una (normales planas, sin compartir vertices entre caras)
    struct Cara { float nx, ny, nz; float verts[4][3]; };
    Cara caras[6] = {
        {  0,  0,  1, { {-ax,-ay, az}, { ax,-ay, az}, { ax, ay, az}, {-ax, ay, az} } }, // frente
        {  0,  0, -1, { { ax,-ay,-az}, {-ax,-ay,-az}, {-ax, ay,-az}, { ax, ay,-az} } }, // atras
        { -1,  0,  0, { {-ax,-ay,-az}, {-ax,-ay, az}, {-ax, ay, az}, {-ax, ay,-az} } }, // izquierda
        {  1,  0,  0, { { ax,-ay, az}, { ax,-ay,-az}, { ax, ay,-az}, { ax, ay, az} } }, // derecha
        {  0,  1,  0, { {-ax, ay, az}, { ax, ay, az}, { ax, ay,-az}, {-ax, ay,-az} } }, // arriba
        {  0, -1,  0, { {-ax,-ay,-az}, { ax,-ay,-az}, { ax,-ay, az}, {-ax,-ay, az} } }, // abajo
    };

    for (const auto& cara : caras) {
        unsigned int base = static_cast<unsigned int>(malla.vertices.size());
        for (const auto& v : cara.verts) agregarVertice(malla, v[0], v[1], v[2], cara.nx, cara.ny, cara.nz);
        malla.indices.insert(malla.indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });
    }
    return malla;
}

MallaSimple Primitivas::crearEsfera(int paralelos, int meridianos) {
    MallaSimple malla;

    for (int i = 0; i <= paralelos; ++i) {
        float phi = PI * static_cast<float>(i) / paralelos;       // 0 (polo sur) .. pi (polo norte), lo dejamos y en [-1,1]
        float y = std::cos(phi);
        float radio = std::sin(phi);

        for (int j = 0; j <= meridianos; ++j) {
            float theta = 2.0f * PI * static_cast<float>(j) / meridianos;
            float x = radio * std::cos(theta);
            float z = radio * std::sin(theta);
            agregarVertice(malla, x, y, z, x, y, z); // en esfera unitaria, normal == posicion
        }
    }

    for (int i = 0; i < paralelos; ++i) {
        for (int j = 0; j < meridianos; ++j) {
            unsigned int actual = i * (meridianos + 1) + j;
            unsigned int siguiente = actual + meridianos + 1;

            malla.indices.insert(malla.indices.end(), { actual, siguiente, actual + 1 });
            malla.indices.insert(malla.indices.end(), { actual + 1, siguiente, siguiente + 1 });
        }
    }
    return malla;
}

MallaSimple Primitivas::crearCono(float radioBase, float altura, int segmentos) {
    MallaSimple malla;

    // Punta (un vertice por segmento para normales suaves tipo cono)
    for (int i = 0; i < segmentos; ++i) {
        float theta0 = 2.0f * PI * static_cast<float>(i) / segmentos;
        float theta1 = 2.0f * PI * static_cast<float>(i + 1) / segmentos;

        float x0 = radioBase * std::cos(theta0), z0 = radioBase * std::sin(theta0);
        float x1 = radioBase * std::cos(theta1), z1 = radioBase * std::sin(theta1);

        // Normal aproximada del lado del cono (promedio simple, suficiente visualmente)
        float nx = std::cos((theta0 + theta1) * 0.5f);
        float nz = std::sin((theta0 + theta1) * 0.5f);
        float ny = radioBase / altura;

        unsigned int base = static_cast<unsigned int>(malla.vertices.size());
        agregarVertice(malla, 0.0f, altura, 0.0f, nx, ny, nz);
        agregarVertice(malla, x0, 0.0f, z0, nx, ny, nz);
        agregarVertice(malla, x1, 0.0f, z1, nx, ny, nz);
        malla.indices.insert(malla.indices.end(), { base, base + 1, base + 2 });
    }

    // Base circular (normal hacia abajo)
    unsigned int centroBase = static_cast<unsigned int>(malla.vertices.size());
    agregarVertice(malla, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
    for (int i = 0; i <= segmentos; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / segmentos;
        agregarVertice(malla, radioBase * std::cos(theta), 0.0f, radioBase * std::sin(theta), 0.0f, -1.0f, 0.0f);
    }
    for (int i = 0; i < segmentos; ++i) {
        malla.indices.insert(malla.indices.end(), { centroBase, centroBase + i + 2, centroBase + i + 1 });
    }

    return malla;
}

MallaSimple Primitivas::crearCilindro(float radio, float altura, int segmentos) {
    MallaSimple malla;

    // Lado
    for (int i = 0; i <= segmentos; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / segmentos;
        float x = radio * std::cos(theta), z = radio * std::sin(theta);
        agregarVertice(malla, x, 0.0f, z, x / radio, 0.0f, z / radio);
        agregarVertice(malla, x, altura, z, x / radio, 0.0f, z / radio);
    }
    for (int i = 0; i < segmentos; ++i) {
        unsigned int base = i * 2;
        malla.indices.insert(malla.indices.end(), { base, base + 1, base + 2 });
        malla.indices.insert(malla.indices.end(), { base + 2, base + 1, base + 3 });
    }

    // Tapas
    unsigned int centroInferior = static_cast<unsigned int>(malla.vertices.size());
    agregarVertice(malla, 0, 0, 0, 0, -1, 0);
    unsigned int inicioInferior = static_cast<unsigned int>(malla.vertices.size());
    for (int i = 0; i <= segmentos; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / segmentos;
        agregarVertice(malla, radio * std::cos(theta), 0.0f, radio * std::sin(theta), 0, -1, 0);
    }
    for (int i = 0; i < segmentos; ++i)
        malla.indices.insert(malla.indices.end(), { centroInferior, inicioInferior + i + 1, inicioInferior + i });

    unsigned int centroSuperior = static_cast<unsigned int>(malla.vertices.size());
    agregarVertice(malla, 0, altura, 0, 0, 1, 0);
    unsigned int inicioSuperior = static_cast<unsigned int>(malla.vertices.size());
    for (int i = 0; i <= segmentos; ++i) {
        float theta = 2.0f * PI * static_cast<float>(i) / segmentos;
        agregarVertice(malla, radio * std::cos(theta), altura, radio * std::sin(theta), 0, 1, 0);
    }
    for (int i = 0; i < segmentos; ++i)
        malla.indices.insert(malla.indices.end(), { centroSuperior, inicioSuperior + i, inicioSuperior + i + 1 });

    return malla;
}
