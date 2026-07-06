#include "Isla.h"
#include <random>

Isla::Isla(Vec3 centro, Shader* shader, unsigned semilla)
    : shader_(shader), centro_(centro) {
    mallaEsfera_ = std::make_unique<ObjetoRenderizable>(Primitivas::crearEsfera(10, 14));

    std::mt19937 rng(semilla);
    std::uniform_real_distribution<float> offset(-3.0f, 3.0f);
    std::uniform_real_distribution<float> escalaXZ(0.6f, 1.6f);
    std::uniform_real_distribution<float> escalaY(0.4f, 0.9f);
    std::uniform_real_distribution<float> tonoGris(0.35f, 0.55f);

    // Base principal de la isla (roca grande y achatada)
    rocas_.push_back({
        Mat4::trasladar(centro_) * Mat4::escalar({3.0f, 1.2f, 3.0f}),
        Vec3{0.45f, 0.42f, 0.35f}
    });

    // Rocas secundarias dispersas alrededor de la base
    const int cantidadRocas = 10;
    for (int i = 0; i < cantidadRocas; ++i) {
        Vec3 posicion = centro_ + Vec3{offset(rng), 0.3f, offset(rng)};
        Mat4 transformacion = Mat4::trasladar(posicion)
                             * Mat4::rotarY(offset(rng))
                             * Mat4::escalar({escalaXZ(rng), escalaY(rng), escalaXZ(rng)});
        float g = tonoGris(rng);
        rocas_.push_back({ transformacion, Vec3{g, g, g * 0.95f} });
    }
}

void Isla::dibujar(const Mat4& vista, const Mat4& proyeccion,
                    const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    shader_->usar();
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setVec3("posCamara", posCamara);
    shader_->setVec3("posLuz", posLuz);
    shader_->setVec3("colorLuz", colorLuz);
    shader_->setVec3("colorAmbiente", Vec3{0.3f, 0.3f, 0.3f});
    shader_->setVec3("colorEspecular", Vec3{0.1f, 0.1f, 0.1f});
    shader_->setFloat("brillo", 4.0f);

    for (const auto& roca : rocas_) {
        shader_->setVec3("colorObjeto", roca.color);
        mallaEsfera_->dibujar(*shader_, roca.transformacion);
    }
}
