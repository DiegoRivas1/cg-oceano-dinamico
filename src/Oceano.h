#pragma once
// Oceano.h
// Construye, actualiza y dibuja toda la superficie del oceano.
// La malla base se sube UNA sola vez a la GPU (posicion x,0,z + uv).
// En cada frame solo actualizamos el uniform "tiempo" y el shader
// recalcula altura + normal por vertice (ver shaders/oceano.vert).
//
// Patron Builder: hay varios parametros opcionales (tamano, separacion,
// fuente de olas, textura, shaders) y queremos una construccion legible
// tipo Oceano::Builder().conTamano(...).conOlas(...).construir();

#include <memory>
#include <string>
#include <vector>

#include "Ola.h"
#include "GestorOlas.h"
#include "Shader.h"
#include "Matematica.h"

class Oceano {
public:
    static constexpr int MAX_OLAS = 32;

    class Builder {
    public:
        Builder& conTamano(int filas, int columnas);
        Builder& conSeparacion(float separacion);
        Builder& conOlas(const FuenteOlas& fuente);
        Builder& conTextura(const std::string& rutaTextura);
        Builder& conShaders(const std::string& rutaVertex, const std::string& rutaFragment);

        std::unique_ptr<Oceano> construir();

    private:
        int filas_ = 100;
        int columnas_ = 100;
        float separacion_ = 1.0f;
        std::vector<Ola> olas_;
        std::string rutaTextura_;
        std::string rutaVertex_ = "shaders/oceano.vert";
        std::string rutaFragment_ = "shaders/oceano.frag";
    };

    ~Oceano();
    Oceano(const Oceano&) = delete;
    Oceano& operator=(const Oceano&) = delete;

    void actualizar(float deltaTiempo);
    void dibujar(const Mat4& vista, const Mat4& proyeccion, const Vec3& posCamara,
                 const Vec3& posLuz, const Vec3& colorLuz) const;

    // Evalua la altura real del oceano en (x,z) en el tiempo acumulado actual.
    // Usada por otros elementos de la escena (ej. el barco) para "flotar".
    float alturaEn(float x, float z) const;

    void alternarTextura()      { usarTextura_ = !usarTextura_; }
    void alternarIluminacion()  { usarIluminacion_ = !usarIluminacion_; }
    void setVelocidad(float v)  { velocidad_ = v; }
    float tiempoActual() const  { return tiempoAcumulado_; }

private:
    Oceano() = default;

    void construirMalla(int filas, int columnas, float separacion);
    void cargarTextura(const std::string& rutaTextura);
    void subirUniformesOlas() const;

    int filas_ = 0, columnas_ = 0;
    float separacion_ = 1.0f;
    std::vector<Ola> olas_;

    unsigned int vao_ = 0, vbo_ = 0, ebo_ = 0;
    unsigned int texturaId_ = 0;
    size_t numIndices_ = 0;

    std::unique_ptr<Shader> shader_;

    float tiempoAcumulado_ = 0.0f;
    float velocidad_ = 1.0f;
    bool usarTextura_ = true;
    bool usarIluminacion_ = true;
};
