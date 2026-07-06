#pragma once
// Isla.h
// Cluster de "rocas" (esferas irregulares por escala no-uniforme) mas una
// base emergente, generado de forma procedural con semilla fija para que
// el resultado sea reproducible entre ejecuciones (util para las capturas
// del informe/video).

#include <memory>
#include <vector>
#include "ElementoEscena.h"
#include "ObjetoRenderizable.h"
#include "Shader.h"

class Isla : public ElementoEscena {
public:
    Isla(Vec3 centro, Shader* shader, unsigned semilla = 7u);

    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    struct Roca { Mat4 transformacion; Vec3 color; };

    Shader* shader_;
    Vec3 centro_;
    std::unique_ptr<ObjetoRenderizable> mallaEsfera_;
    std::vector<Roca> rocas_;
};
