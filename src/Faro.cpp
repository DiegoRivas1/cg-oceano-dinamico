#include "Faro.h"
#include <cmath>

Faro::Faro(Vec3 base, float altura, Shader* shader)
    : shader_(shader), base_(base), altura_(altura) {
    torre_  = std::make_unique<ObjetoRenderizable>(Primitivas::crearCilindro(0.4f, altura));
    cupula_ = std::make_unique<ObjetoRenderizable>(Primitivas::crearCono(0.5f, 0.8f));
}

void Faro::actualizar(float tiempo, const Oceano& /*oceano*/) {
    // Parpadeo suave tipo faro real (oscila entre 0.4 y 1.0)
    parpadeo_ = 0.7f + 0.3f * std::sin(tiempo * 1.5f);
}

Vec3 Faro::posicionLuz() const {
    return { base_.x, base_.y + altura_ + 0.5f, base_.z };
}

void Faro::dibujar(const Mat4& vista, const Mat4& proyeccion,
                    const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    shader_->usar();
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setVec3("posCamara", posCamara);
    shader_->setVec3("posLuz", posLuz);
    shader_->setVec3("colorLuz", colorLuz);
    shader_->setVec3("colorAmbiente", Vec3{0.3f, 0.3f, 0.3f});
    shader_->setVec3("colorEspecular", Vec3{0.5f, 0.5f, 0.5f});
    shader_->setFloat("brillo", 32.0f);

    shader_->setVec3("colorObjeto", Vec3{0.9f, 0.9f, 0.85f});
    torre_->dibujar(*shader_, Mat4::trasladar(base_));

    shader_->setVec3("colorObjeto", colorLuzPropia_ * parpadeo_);
    cupula_->dibujar(*shader_, Mat4::trasladar({base_.x, base_.y + altura_, base_.z}));
}
