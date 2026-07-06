#include "Camara.h"
#include <algorithm>
#include <cmath>

Camara::Camara() { recalcularPosicion(); }

void Camara::recalcularPosicion() {
    posicionCalculada_ = {
        objetivo_.x + radio_ * std::cos(pitch_) * std::cos(yaw_),
        objetivo_.y + radio_ * std::sin(pitch_),
        objetivo_.z + radio_ * std::cos(pitch_) * std::sin(yaw_)
    };
}

void Camara::rotar(float deltaYaw, float deltaPitch) {
    yaw_ += deltaYaw;
    pitch_ = std::clamp(pitch_ + deltaPitch, PITCH_MIN, PITCH_MAX);
    recalcularPosicion();
}

void Camara::acercar(float deltaRadio) {
    radio_ = std::clamp(radio_ + deltaRadio, RADIO_MIN, RADIO_MAX);
    recalcularPosicion();
}

Mat4 Camara::obtenerVista() const {
    return Mat4::mirarHacia(posicionCalculada_, objetivo_, {0.0f, 1.0f, 0.0f});
}
