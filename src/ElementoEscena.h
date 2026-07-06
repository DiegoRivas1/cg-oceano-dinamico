#pragma once
// ElementoEscena.h
// Interfaz comun para los elementos adicionales (barco, isla, faro, skybox).
// Escena los guarda como std::vector<std::unique_ptr<ElementoEscena>> sin
// conocer el tipo concreto (polimorfismo + Factory).

#include "Matematica.h"

class Oceano; // fwd decl: algunos elementos consultan alturaEn() del oceano

class ElementoEscena {
public:
    virtual ~ElementoEscena() = default;

    // No todos los elementos necesitan actualizarse (el faro y la isla son
    // estaticos); por eso tiene implementacion vacia por defecto.
    virtual void actualizar(float tiempo, const Oceano& oceano) {}

    virtual void dibujar(const Mat4& vista, const Mat4& proyeccion,
                          const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const = 0;
};
