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
    static std::unique_ptr<ElementoEscena> crear(TipoElemento tipo, Shader* shaderBasico, Vec3 posicion = {});
};
