#ifndef CG_OCEANO_DINAMICO_ESCENA_H
#define CG_OCEANO_DINAMICO_ESCENA_H

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
    // Rango de "severidad": con amplitud/frecuencia bajas, la pendiente que
    // genera no llega a UMBRAL_VOLCADO (solo tambalea fuerte); con valores
    // altos, si lo supera y vuelca al barco. Asi la variabilidad es real,
    // no una probabilidad artificial superpuesta.
    std::uniform_real_distribution<float> amplitudErraticaAleatoria_{2.0f, 4.5f};
    std::uniform_real_distribution<float> frecuenciaErraticaAleatoria_{0.20f, 0.38f};

    // Barcos: se mantiene un cupo total (incluyendo el barco inicial). Cuando
    // uno se hunde y Escena lo elimina de elementos_, el cupo se libera y
    // el temporizador vuelve a intentar spawnear otro mas adelante.
    static constexpr int MAX_BARCOS = 5;
    float tiempoParaProximoBarco_ = 5.0f;
    std::uniform_real_distribution<float> posicionBarcoAleatoria_{-14.0f, 14.0f};
    std::uniform_real_distribution<float> radioBarcoAleatorio_{5.0f, 11.0f};
    std::uniform_real_distribution<float> velocidadBarcoAleatoria_{0.08f, 0.22f};
    std::uniform_real_distribution<float> anguloBarcoAleatorio_{0.0f, 6.2831853f};
    std::uniform_real_distribution<float> intervaloBarcoAleatorio_{8.0f, 18.0f};
    std::uniform_int_distribution<int> signoGiroAleatorio_{0, 1};

    // Umbral de velocidad relativa de impacto a partir del cual un barco se
    // rompe (se hunde) en vez de solo rebotar/cambiar de sentido.
    static constexpr float UMBRAL_IMPACTO_ROMPE = 1.6f;

    void gestionarColisionesBarcos();
    void gestionarSpawnBarcos(float deltaTiempo);
    void eliminarBarcosHundidos();
};

#endif //CG_OCEANO_DINAMICO_ESCENA_H