#ifndef CG_OCEANO_DINAMICO_ELEMENTOESCENAFACTORY_H
#define CG_OCEANO_DINAMICO_ELEMENTOESCENAFACTORY_H
#pragma once
// ElementoEscenaFactory.h
// Patron Factory Method: centraliza la creacion de cada tipo de elemento
// adicional, ocultando a Escena los detalles de construccion (que shader
// usa, con que semilla, en que posicion por defecto, etc).

#include <memory>
#include "ElementoEscena.h"
#include "Shader.h"

enum class TipoElemento { Barco, Isla, Faro, Skybox };

class ElementoEscenaFactory {
public:
    // shaderBasico es compartido (no-owning) entre Barco/Isla/Faro; Skybox
    // trae su propio shader porque usa un vertex/fragment distinto.
    // radioOrbita/velocidadAngular/anguloInicial solo aplican a TipoElemento::Barco;
    // el resto de elementos los ignora.
    static std::unique_ptr<ElementoEscena> crear(TipoElemento tipo, Shader* shaderBasico, Vec3 posicion = {},
                                                  float radioOrbita = 6.0f, float velocidadAngular = 0.15f,
                                                  float anguloInicial = 0.0f);
};

#endif //CG_OCEANO_DINAMICO_ELEMENTOESCENAFACTORY_H