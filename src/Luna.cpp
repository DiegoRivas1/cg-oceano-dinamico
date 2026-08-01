#include "Luna.h"
#include "OBJLoader.h"
#include "CicloDiaNoche.h"
#include <cmath>
#include <fstream>
#include <iostream>

namespace {
constexpr const char* RUTA_MODELO_OBJ = "models/luna/luna.obj";
constexpr const char* RUTA_TEXTURA = "models/luna/luna_textura.jpg";
}

Luna::Luna(Shader* shaderTexturado) : shaderTexturado_(shaderTexturado) {
    std::ifstream prueba(RUTA_MODELO_OBJ);
    modeloValido_ = prueba.good();
    if (!modeloValido_) {
        std::cout << "[Luna] No se encontro " << RUTA_MODELO_OBJ << " - la luna no se dibuja.\n";
        return;
    }

    MallaOBJ malla = OBJLoader::cargar(RUTA_MODELO_OBJ);
    if (malla.vertices.empty()) {
        std::cerr << "[Luna] " << RUTA_MODELO_OBJ << " no tiene triangulos.\n";
        modeloValido_ = false;
        return;
    }

    OBJLoader::normalizarEscala(malla, /*tamanoObjetivo*/ 15.0f);
    // ya NO crea su propio shaderTexturado_, usa el que le paso Escena
    modelo_ = std::make_unique<ObjetoRenderizableTexturado>(malla, RUTA_TEXTURA);
}

void Luna::actualizar(float tiempo, const Oceano& /*oceano*/) {
    float angulo = CicloDiaNoche::anguloLuna(tiempo);
    posicion_ = {
        CicloDiaNoche::RADIO_ARCO_CIELO * std::cos(angulo),
        CicloDiaNoche::RADIO_ARCO_CIELO * std::sin(angulo),
        0.0f
    };
}

void Luna::dibujar(const Mat4& vista, const Mat4& proyeccion,
                    const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    if (!modeloValido_ || posicion_.y < -20.0f) return;

    shaderTexturado_->usar();
    shaderTexturado_->setMat4("vista", vista);
    shaderTexturado_->setMat4("proyeccion", proyeccion);
    shaderTexturado_->setVec3("posCamara", posCamara);
    shaderTexturado_->setVec3("posLuz", posLuz);
    shaderTexturado_->setVec3("colorLuz", colorLuz);
    shaderTexturado_->setVec3("colorAmbiente", Vec3{0.55f, 0.55f, 0.6f});
    shaderTexturado_->setVec3("colorEspecular", Vec3{0.0f, 0.0f, 0.0f});
    shaderTexturado_->setFloat("brillo", 1.0f);
    shaderTexturado_->setBool("esAutoiluminado", true);

    modelo_->dibujar(*shaderTexturado_, Mat4::trasladar(posicion_));
}