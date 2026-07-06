#pragma once
// Barco.h
// Casco + mastil + vela armados con primitivas. En actualizar() muestrea
// Oceano::alturaEn() en su posicion (x,z) para "flotar" sobre la ola real,
// y se orienta levemente segun la pendiente local (efecto de balanceo).

#include <memory>
#include "ElementoEscena.h"
#include "ObjetoRenderizable.h"
#include "Shader.h"

class Barco : public ElementoEscena {
public:
    Barco(Vec3 posicionXZ, Shader* shader);

    void actualizar(float tiempo, const Oceano& oceano) override;
    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    Shader* shader_; // no-owning: Escena es duena del shader compartido
    std::unique_ptr<ObjetoRenderizable> casco_;
    std::unique_ptr<ObjetoRenderizable> mastil_;
    std::unique_ptr<ObjetoRenderizable> vela_;

    Vec3 posicionBase_;   // (x, _, z) fijo; la y se recalcula cada frame
    float alturaActual_ = 0.0f;
    float balanceo_ = 0.0f; // angulo de cabeceo, en radianes
};
