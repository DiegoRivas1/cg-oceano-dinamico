#ifndef CG_OCEANO_DINAMICO_BARCO_H
#define CG_OCEANO_DINAMICO_BARCO_H
//#pragma once
// Barco.h
// Casco + mastil + vela armados con primitivas (fallback), O el modelo real
// "Medieval Boat" cargado desde models/barco_medieval/barco.obj si el
// archivo existe (ver Barco::Barco). En actualizar() muestrea
// Oceano::alturaEn()/gradienteEn() en su posicion (x,z) para "flotar" sobre
// la ola real, con roll+pitch calculados de la pendiente analitica.

#include <memory>
#include "ElementoEscena.h"
#include "ObjetoRenderizable.h"
#include "ObjetoRenderizableTexturado.h"
#include "Shader.h"

class Barco : public ElementoEscena {
public:
    // centroOrbita: punto alrededor del cual navega. radioOrbita/velocidadAngular
    // controlan el circulo de navegacion; anguloInicial desfasa el punto de partida
    // (util para que varios barcos no arranquen todos en el mismo punto).
    Barco(Vec3 centroOrbita, Shader* shader,
          float radioOrbita = 6.0f, float velocidadAngular = 0.15f, float anguloInicial = 0.0f);

    void actualizar(float tiempo, const Oceano& oceano) override;
    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    Shader* shader_;

    std::unique_ptr<ObjetoRenderizable> casco_;
    std::unique_ptr<ObjetoRenderizable> mastil_;
    std::unique_ptr<ObjetoRenderizable> vela_;

    bool usaModeloReal_ = false;
    std::unique_ptr<Shader> shaderTexturado_;
    std::unique_ptr<ObjetoRenderizableTexturado> modeloReal_;

    // --- Movimiento: navegacion circular alrededor de centroOrbita_ ---
    Vec3 centroOrbita_;
    float radioOrbita_;
    float velocidadAngular_;
    float anguloInicial_;
    float anguloActual_ = 0.0f;
    float yaw_ = 0.0f; // orientacion (proa) segun direccion de movimiento

    Vec3 posicionBase_;   // (x, _, z) recalculado cada frame desde la orbita
    float alturaActual_ = 0.0f;
    float roll_ = 0.0f;
    float pitch_ = 0.0f;
};

#endif //CG_OCEANO_DINAMICO_BARCO_H
