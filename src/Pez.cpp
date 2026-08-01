#include "Pez.h"
#include "Oceano.h"
#include "OBJLoader.h"
#include <cmath>
#include <fstream>
#include <iostream>

namespace {
constexpr const char* RUTA_MODELO_OBJ = "models/pescado/pez.obj";
constexpr const char* RUTA_TEXTURA = "models/pescado/fish.jpg";
constexpr float PI = 3.14159265f;
}

Pez::Pez(Vec3 centroZona, float profundidad, float radioOrbita, float velocidadAngular, float anguloInicial,
         Shader* shaderTexturado, ObjetoRenderizableTexturado* modeloCompartido)
    : centroZona_(centroZona), profundidad_(profundidad), radioOrbita_(radioOrbita),
      velocidadAngular_(velocidadAngular), anguloInicial_(anguloInicial),
      shaderTexturado_(shaderTexturado), modelo_(modeloCompartido) {
    modeloValido_ = (modelo_ != nullptr);
}

void Pez::actualizar(float tiempo, const Oceano& oceano) {
    if (!modeloValido_) return;

    anguloActual_ = anguloInicial_ + tiempo * velocidadAngular_;
    posicionActual_.x = centroZona_.x + radioOrbita_ * std::cos(anguloActual_);
    posicionActual_.z = centroZona_.z + radioOrbita_ * std::sin(anguloActual_);

    // Profundidad relativa a la superficie REAL del agua (que ya sube y baja
    // con las olas), mas un pequeno ondulado vertical para el nado.
    float ondulado = std::sin(tiempo * 2.0f + anguloInicial_) * 0.15f;
    float superficie = oceano.alturaEn(posicionActual_.x, posicionActual_.z);
    posicionActual_.y = superficie - profundidad_ + ondulado;

    float signoGiro = (velocidadAngular_ >= 0.0f) ? 1.0f : -1.0f;
    yaw_ = anguloActual_ - signoGiro * (PI * 0.5f);
}

void Pez::dibujar(const Mat4& vista, const Mat4& proyeccion,
                   const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    if (!modeloValido_) return;

    shaderTexturado_->usar();
    shaderTexturado_->setMat4("vista", vista);
    shaderTexturado_->setMat4("proyeccion", proyeccion);
    shaderTexturado_->setVec3("posCamara", posCamara);
    shaderTexturado_->setVec3("posLuz", posLuz);
    shaderTexturado_->setVec3("colorLuz", colorLuz);
    shaderTexturado_->setVec3("colorAmbiente", Vec3{0.5f, 0.5f, 0.5f});
    shaderTexturado_->setVec3("colorEspecular", Vec3{0.15f, 0.15f, 0.15f});
    shaderTexturado_->setFloat("brillo", 8.0f);
    shaderTexturado_->setBool("esAutoiluminado", false);

    Mat4 transformacion = Mat4::trasladar(posicionActual_) * Mat4::rotarY(yaw_);
    modelo_->dibujar(*shaderTexturado_, transformacion);
}