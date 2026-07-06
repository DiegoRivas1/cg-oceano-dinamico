#include "ElementoEscenaFactory.h"
#include "Barco.h"
#include "Isla.h"
#include "Faro.h"
#include "Skybox.h"

std::unique_ptr<ElementoEscena> ElementoEscenaFactory::crear(TipoElemento tipo, Shader* shaderBasico, Vec3 posicion) {
    switch (tipo) {
        case TipoElemento::Barco:
            return std::make_unique<Barco>(posicion, shaderBasico);
        case TipoElemento::Isla:
            return std::make_unique<Isla>(posicion, shaderBasico);
        case TipoElemento::Faro:
            return std::make_unique<Faro>(posicion, /*altura*/ 3.5f, shaderBasico);
        case TipoElemento::Skybox:
            return std::make_unique<Skybox>();
    }
    return nullptr;
}
