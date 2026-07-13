#pragma once
// Escena.h
// Punto de composicion final: agrupa Oceano, la camara, la luz principal
// y los elementos adicionales (creados via ElementoEscenaFactory). main.cpp
// solo llama a Escena::actualizar()/dibujar(); no conoce clases individuales.

#include <memory>
#include <random>
#include <vector>
#include "Oceano.h"
#include "Camara.h"
#include "ElementoEscena.h"
#include "Shader.h"

class Faro; // fwd decl: la escena consulta su posicion de luz secundaria

class Escena {
public:
    Escena();

    void actualizar(float deltaTiempo);
    void dibujar(float aspecto) const;

    Camara& camara() { return camara_; }
    Oceano& oceano() { return *oceano_; }

private:
    std::unique_ptr<Oceano> oceano_;
    std::unique_ptr<Shader> shaderBasico_;
    std::vector<std::unique_ptr<ElementoEscena>> elementos_;
    Camara camara_;

    Faro* faro_ = nullptr; // no-owning, puntero al elemento dentro de elementos_

    Vec3 posLuzPrincipal_{50.0f, 80.0f, -30.0f};
    Vec3 colorLuzPrincipal_{1.0f, 0.98f, 0.9f};

    float tiempoTotal_ = 0.0f;

    // Dispara Oceano::activarOlaErratica() cada cierto intervalo aleatorio,
    // simulando que de vez en cuando aparece una ola fuera de lo comun.
    float tiempoParaProximaErratica_ = 8.0f; // primera aparicion a los 8s, para verla pronto
    std::mt19937 generadorErratica_{std::random_device{}()};
    std::uniform_real_distribution<float> intervaloErratica_{18.0f, 35.0f};
    std::uniform_real_distribution<float> direccionErratica_{0.0f, 6.2831853f};
};
