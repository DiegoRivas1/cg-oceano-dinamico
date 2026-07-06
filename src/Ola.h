#pragma once
// Ola.h
// Representa una unica ola sinusoidal segun la formula del enunciado:
//   h(x,z,t) = A * cos( k*(x*cos(d) + z*sin(d)) - 2*pi*f*t + p )
//   k = 4*pi^2*f^2 / 9.81
//
// La misma formula se evalua tanto aqui (CPU, usada por ejemplo para hacer
// flotar al barco) como en el vertex shader (GPU, usada para deformar la malla).

class Ola {
public:
    Ola(float amplitud, float direccionRad, float frecuencia, float fase);

    // Altura de esta ola sola, en el punto (x,z) en el instante t.
    float altura(float x, float z, float tiempo) const;

    float amplitud() const   { return amplitud_; }
    float direccion() const  { return direccion_; }
    float frecuencia() const { return frecuencia_; }
    float fase() const       { return fase_; }
    float numeroOnda() const { return numeroOnda_; }

private:
    float amplitud_;
    float direccion_;   // radianes
    float frecuencia_;
    float fase_;
    float numeroOnda_;  // k, calculado a partir de la frecuencia

    static constexpr float GRAVEDAD = 9.81f;
    static float calcularNumeroOnda(float frecuencia);
};
