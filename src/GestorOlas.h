#pragma once
// GestorOlas.h
// Patron Strategy: Oceano no sabe de donde vienen las olas, solo pide
// obtenerOlas() a una FuenteOlas. Esto permite intercambiar el origen
// (definicion manual en codigo vs carga desde data/spectrum.txt) sin
// tocar la clase Oceano.

#include <memory>
#include <string>
#include <vector>
#include "Ola.h"

class FuenteOlas {
public:
    virtual ~FuenteOlas() = default;
    virtual std::vector<Ola> obtenerOlas() const = 0;
};

// Olas definidas a mano en codigo.
class OlasManual : public FuenteOlas {
public:
    explicit OlasManual(std::vector<Ola> olas) : olas_(std::move(olas)) {}
    std::vector<Ola> obtenerOlas() const override { return olas_; }

private:
    std::vector<Ola> olas_;
};

// Olas cargadas desde un archivo de texto con formato:
//   amplitud  direccion(rad)  frecuencia
// (una ola por linea, separadas por espacio o tabulador).
// El archivo no trae fase, asi que se genera aleatoriamente con una
// semilla fija para que la animacion sea reproducible entre ejecuciones.
class OlasDesdeArchivo : public FuenteOlas {
public:
    explicit OlasDesdeArchivo(std::string rutaArchivo, unsigned semilla = 42u);
    std::vector<Ola> obtenerOlas() const override;

private:
    std::string rutaArchivo_;
    unsigned semilla_;
};
