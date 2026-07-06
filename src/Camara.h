#pragma once
// Camara.h
// Camara orbital: rota alrededor de un punto objetivo (origen) usando
// coordenadas esfericas (radio, yaw, pitch). Permite rotar, acercar/alejar
// y consultar la matriz de vista, tal como pide el enunciado (2.5).

#include "Matematica.h"

class Camara {
public:
    Camara();

    void rotar(float deltaYaw, float deltaPitch);
    void acercar(float deltaRadio);

    Mat4 obtenerVista() const;
    Vec3 posicion() const { return posicionCalculada_; }

private:
    void recalcularPosicion();

    float radio_ = 25.0f;
    float yaw_ = -1.2f;    // radianes
    float pitch_ = 0.5f;   // radianes
    Vec3 objetivo_{0.0f, 0.0f, 0.0f};
    Vec3 posicionCalculada_;

    static constexpr float PITCH_MIN = 0.1f;
    static constexpr float PITCH_MAX = 1.5f;
    static constexpr float RADIO_MIN = 3.0f;
    static constexpr float RADIO_MAX = 120.0f;
};
