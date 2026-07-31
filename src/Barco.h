#ifndef CG_OCEANO_DINAMICO_BARCO_H
#define CG_OCEANO_DINAMICO_BARCO_H
#pragma once
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

    // --- API para que Escena maneje colisiones entre barcos ---
    Vec3 posicion() const { return posicionBase_; }
    float radioColision() const { return 1.3f; }
    float velocidadLineal() const { return radioOrbita_ * velocidadAngular_; }

    void invertirDireccion();          // colision leve: rebota y cambia de sentido
    void romper();                     // colision fuerte: empieza a hundirse
    bool listoParaEliminar() const;    // true cuando termino de hundirse -> Escena lo quita de elementos_

    bool enCooldownColision() const { return cooldownColision_ > 0.0f; }
    void marcarCooldownColision() { cooldownColision_ = COOLDOWN_COLISION; }

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
    float yaw_ = 0.0f;         // orientacion VISUAL actual de la proa (persigue a yawObjetivo_)
    float yawObjetivo_ = 0.0f; // hacia donde deberia apuntar segun el movimiento actual
    bool yawInicializado_ = false;

    Vec3 posicionBase_;   // (x, _, z) recalculado cada frame desde la orbita
    float alturaActual_ = 0.0f;
    float roll_ = 0.0f;
    float pitch_ = 0.0f;

    // --- Maquina de estados: Normal -> Volcado (ola fuerte) -> Hundiendose -> eliminado ---
    // o directamente Normal -> Hundiendose (colision fuerte, via romper()).
    enum class EstadoBarco { Normal, Volcado, Hundiendose };
    EstadoBarco estado_ = EstadoBarco::Normal;
    float tiempoEnEstado_ = 0.0f;
    float tiempoAnteriorAbsoluto_ = -1.0f; // para derivar deltaTiempo (actualizar() solo recibe tiempo absoluto)
    float cooldownColision_ = 0.0f;

    static constexpr float UMBRAL_VOLCADO = 0.70f;      // ~40 grados de roll/pitch -> se considera volcado
    static constexpr float VELOCIDAD_GIRO_MAX = 1.5f;   // rad/s, que tan rapido puede girar la proa visualmente
    static constexpr float DURACION_VOLCADO = 2.0f;     // segundos volcado antes de empezar a hundirse
    static constexpr float DURACION_HUNDIMIENTO = 3.0f; // segundos hundiendose antes de poder eliminarse
    static constexpr float COOLDOWN_COLISION = 1.5f;    // evita re-colisionar el mismo par cada frame
};
#endif //CG_OCEANO_DINAMICO_BARCO_H
