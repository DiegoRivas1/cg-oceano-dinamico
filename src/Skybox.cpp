#include "Skybox.h"
#include "ObjetoRenderizable.h"
#include "Primitivas.h"
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
    shader_->setVec3("colorCima", Vec3{0.30f, 0.55f, 0.85f});
    shader_->setVec3("colorHorizonte", Vec3{0.85f, 0.85f, 0.80f});

    esfera_->dibujar(*shader_, Mat4::escalar({radio_, radio_, radio_}));

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
