#pragma once
// Skybox.h
// En vez de un cubemap con 6 texturas externas (que no tenemos como asset),
// usamos una cupula grande (media esfera invertida) con gradiente de color
// entre horizonte y cenit, resuelto en skybox.frag. Se dibuja con
// glDepthFunc(GL_LEQUAL) y sin escribir al depth buffer del fondo real,
// para que siempre quede "detras" de todo.

#include <memory>
#include "ElementoEscena.h"
#include "Shader.h"

class ObjetoRenderizable;

class Skybox : public ElementoEscena {
public:
    explicit Skybox(float radio = 200.0f);
    ~Skybox() override;

    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    std::unique_ptr<Shader> shader_;
    std::unique_ptr<ObjetoRenderizable> esfera_;
    float radio_;
};
