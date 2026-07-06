#include "GestorOlas.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

OlasDesdeArchivo::OlasDesdeArchivo(std::string rutaArchivo, unsigned semilla)
    : rutaArchivo_(std::move(rutaArchivo)), semilla_(semilla) {}

std::vector<Ola> OlasDesdeArchivo::obtenerOlas() const {
    std::vector<Ola> olas;
    constexpr float PI = 3.14159265358979323846f;

    std::ifstream archivo(rutaArchivo_);
    if (!archivo.is_open()) {
        std::cerr << "[GestorOlas] No se pudo abrir: " << rutaArchivo_ << "\n";
        return olas;
    }

    std::mt19937 generador(semilla_);
    std::uniform_real_distribution<float> faseAleatoria(0.0f, 2.0f * PI);

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        std::istringstream stream(linea);
        float amplitud, direccion, frecuencia;
        if (!(stream >> amplitud >> direccion >> frecuencia)) {
            std::cerr << "[GestorOlas] Linea invalida, se omite: " << linea << "\n";
            continue;
        }

        float fase = faseAleatoria(generador);
        olas.emplace_back(amplitud, direccion, frecuencia, fase);
    }

    std::cout << "[GestorOlas] " << olas.size() << " olas cargadas desde " << rutaArchivo_ << "\n";
    return olas;
}
