#include "Ola.h"
#include <cmath>

namespace {
constexpr float PI = 3.14159265358979323846f;
}

Ola::Ola(float amplitud, float direccionRad, float frecuencia, float fase)
    : amplitud_(amplitud),
      direccion_(direccionRad),
      frecuencia_(frecuencia),
      fase_(fase),
      numeroOnda_(calcularNumeroOnda(frecuencia)) {}

float Ola::calcularNumeroOnda(float frecuencia) {
    return (4.0f * PI * PI * frecuencia * frecuencia) / GRAVEDAD;
}

float Ola::altura(float x, float z, float tiempo) const {
    const float dosPi = 2.0f * PI;
    float proyeccion = x * std::cos(direccion_) + z * std::sin(direccion_);
    float fase_i = numeroOnda_ * proyeccion - dosPi * frecuencia_ * tiempo + fase_;
    return amplitud_ * std::cos(fase_i);
}

void Ola::gradiente(float x, float z, float tiempo, float& dHdx, float& dHdz) const {
    const float dosPi = 2.0f * PI;
    float dirX = std::cos(direccion_);
    float dirZ = std::sin(direccion_);
    float proyeccion = x * dirX + z * dirZ;
    float fase_i = numeroOnda_ * proyeccion - dosPi * frecuencia_ * tiempo + fase_;

    // d/dx[A*cos(k*(x*dx+z*dz) - w*t + p)] = -A*k*dx*sin(...)  (igual que en oceano.vert)
    float derivComun = -amplitud_ * numeroOnda_ * std::sin(fase_i);
    dHdx += derivComun * dirX;
    dHdz += derivComun * dirZ;
}
