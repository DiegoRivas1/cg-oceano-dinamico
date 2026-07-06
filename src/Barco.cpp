#include "Barco.h"
#include "Oceano.h"
#include <cmath>

Barco::Barco(Vec3 posicionXZ, Shader* shader)
    : shader_(shader), posicionBase_(posicionXZ) {
    casco_  = std::make_unique<ObjetoRenderizable>(Primitivas::crearCaja(2.5f, 0.8f, 1.0f));
    mastil_ = std::make_unique<ObjetoRenderizable>(Primitivas::crearCilindro(0.06f, 2.2f));
    vela_   = std::make_unique<ObjetoRenderizable>(Primitivas::crearCaja(0.05f, 1.4f, 0.9f));
}

void Barco::actualizar(float /*tiempo*/, const Oceano& oceano) {
    // Muestreamos la altura real del oceano bajo el barco para que flote
    // exactamente sobre la superficie deformada por las olas.
    alturaActual_ = oceano.alturaEn(posicionBase_.x, posicionBase_.z);

    // Balanceo simple: comparamos la altura un poco adelante/atras del barco
    // para estimar la pendiente local y usarla como angulo de cabeceo.
    const float delta = 0.5f;
    float alturaAdelante = oceano.alturaEn(posicionBase_.x, posicionBase_.z + delta);
    float alturaAtras    = oceano.alturaEn(posicionBase_.x, posicionBase_.z - delta);
    balanceo_ = std::atan2(alturaAdelante - alturaAtras, 2.0f * delta) * 0.5f;
}

void Barco::dibujar(const Mat4& vista, const Mat4& proyeccion,
                     const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    shader_->usar();
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setVec3("posCamara", posCamara);
    shader_->setVec3("posLuz", posLuz);
    shader_->setVec3("colorLuz", colorLuz);
    shader_->setVec3("colorAmbiente", Vec3{0.25f, 0.25f, 0.25f});
    shader_->setVec3("colorEspecular", Vec3{0.4f, 0.4f, 0.4f});
    shader_->setFloat("brillo", 16.0f);

    Mat4 flotacion = Mat4::trasladar({posicionBase_.x, alturaActual_ + 0.4f, posicionBase_.z})
                    * Mat4::rotarY(0.0f);

    shader_->setVec3("colorObjeto", Vec3{0.35f, 0.22f, 0.10f}); // casco: madera
    casco_->dibujar(*shader_, flotacion);

    shader_->setVec3("colorObjeto", Vec3{0.30f, 0.20f, 0.10f});
    mastil_->dibujar(*shader_, flotacion * Mat4::trasladar({0.0f, 0.4f, 0.0f}));

    shader_->setVec3("colorObjeto", Vec3{0.95f, 0.95f, 0.90f}); // vela: blanco
    vela_->dibujar(*shader_, flotacion * Mat4::trasladar({0.0f, 0.9f, 0.0f}));
}
