#pragma once
// Faro.h
// Torre (cilindro) + cupula (cono) sobre la isla. Ademas de decorativo,
// expone su posicion como una SEGUNDA fuente de luz puntual (ademas de la
// luz principal de la escena), superando el minimo de "al menos una luz"
// pedido en el enunciado.

#include <memory>
#include "ElementoEscena.h"
#include "ObjetoRenderizable.h"
#include "Shader.h"

class Faro : public ElementoEscena {
public:
    Faro(Vec3 base, float altura, Shader* shader);

    void actualizar(float tiempo, const Oceano& oceano) override;
    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

    Vec3 posicionLuz() const;
    Vec3 colorLuz() const { return colorLuzPropia_; }

private:
    Shader* shader_;
    Vec3 base_;
    float altura_;
    std::unique_ptr<ObjetoRenderizable> torre_;
    std::unique_ptr<ObjetoRenderizable> cupula_;
    Vec3 colorLuzPropia_{1.0f, 0.9f, 0.6f}; // luz calida tipo faro
    float parpadeo_ = 1.0f;
};
