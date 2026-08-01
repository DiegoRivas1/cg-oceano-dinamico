#include "Skybox.h"
#include "ObjetoRenderizable.h"
#include "Primitivas.h"
#include "CicloDiaNoche.h"
#include <glad/glad.h>

Skybox::Skybox(float radio) : radio_(radio) {
    shader_ = std::make_unique<Shader>("shaders/skybox.vert", "shaders/skybox.frag");
    esfera_ = std::make_unique<ObjetoRenderizable>(Primitivas::crearEsfera(12, 20));
}

Skybox::~Skybox() = default;

void Skybox::dibujar(const Mat4& vista, const Mat4& proyeccion,
                      const Vec3& /*posCamara*/, const Vec3& /*posLuz*/, const Vec3& /*colorLuz*/) const {
    // GL_LEQUAL para que la cupula pase el test de profundidad incluso
    // cuando gl_Position.z queda exactamente en el far plane.
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE); // no escribe profundidad: siempre queda al fondo

    shader_->usar();
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setVec3("colorCima", colorCimaActual_);
    shader_->setVec3("colorHorizonte", colorHorizonteActual_);

    esfera_->dibujar(*shader_, Mat4::escalar({radio_, radio_, radio_}));

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void Skybox::actualizar(float tiempo, const Oceano& /*oceano*/) {
    float f = CicloDiaNoche::factorDia(tiempo);

    Vec3 cimaDia{0.30f, 0.55f, 0.85f};
    Vec3 horizonteDia{0.85f, 0.85f, 0.80f};
    Vec3 cimaNoche{0.02f, 0.03f, 0.12f};
    Vec3 horizonteNoche{0.08f, 0.08f, 0.20f};

    colorCimaActual_      = cimaNoche      + (cimaDia - cimaNoche) * f;
    colorHorizonteActual_ = horizonteNoche + (horizonteDia - horizonteNoche) * f;
}