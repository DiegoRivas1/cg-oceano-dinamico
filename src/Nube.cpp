#include "Nube.h"
#include <random>
#include <cmath>

Nube::Nube(Vec3 posicionInicial, Shader* shader, float velocidadDeriva, unsigned semilla)
    : shader_(shader), posicionInicial_(posicionInicial), posicionActual_(posicionInicial),
      velocidadDeriva_(velocidadDeriva) {

    mallaEsfera_ = std::make_unique<ObjetoRenderizable>(Primitivas::crearEsfera(8, 12));

    std::mt19937 rng(semilla);
    std::uniform_real_distribution<float> offsetXZ(-4.0f, 4.0f);
    std::uniform_real_distribution<float> offsetY(-0.8f, 0.8f);
    std::uniform_real_distribution<float> escalaAleatoria(2.0f, 4.5f);

    const int cantidadBultos = 5;
    for (int i = 0; i < cantidadBultos; ++i) {
        Vec3 offset{offsetXZ(rng), offsetY(rng), offsetXZ(rng)};
        float e = escalaAleatoria(rng);
        bultosLocales_.push_back(Mat4::trasladar(offset) * Mat4::escalar({e, e * 0.6f, e}));
    }
}

void Nube::actualizar(float tiempo, const Oceano& /*oceano*/) {
    // Posicion en funcion DIRECTA del tiempo absoluto (no acumulamos delta),
    // envuelta en [-LIMITE_DERIVA, LIMITE_DERIVA] para que reaparezca del
    // otro lado en vez de alejarse para siempre.
    float x = posicionInicial_.x + velocidadDeriva_ * tiempo;
    float rango = 2.0f * LIMITE_DERIVA;
    float xEnvuelta = std::fmod(x + LIMITE_DERIVA, rango);
    if (xEnvuelta < 0.0f) xEnvuelta += rango;
    posicionActual_.x = xEnvuelta - LIMITE_DERIVA;
}

void Nube::dibujar(const Mat4& vista, const Mat4& proyeccion,
                    const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    shader_->usar();
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setVec3("posCamara", posCamara);
    shader_->setVec3("posLuz", posLuz);
    shader_->setVec3("colorLuz", colorLuz);
    shader_->setVec3("colorObjeto", Vec3{0.95f, 0.95f, 0.97f});
    shader_->setVec3("colorAmbiente", Vec3{0.9f, 0.9f, 0.9f});
    shader_->setVec3("colorEspecular", Vec3{0.0f, 0.0f, 0.0f});
    shader_->setFloat("brillo", 1.0f);

    Vec3 posicionFija{posicionActual_.x, posicionInicial_.y, posicionInicial_.z};
    for (const auto& bultoLocal : bultosLocales_) {
        mallaEsfera_->dibujar(*shader_, Mat4::trasladar(posicionFija) * bultoLocal);
    }
}