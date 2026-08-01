
#ifndef CG_OCEANO_DINAMICO_NUBE_H
#define CG_OCEANO_DINAMICO_NUBE_H
// Nube.h
// Nube procedural (cluster de esferas achatadas, mismo espiritu que Isla)
// que deriva horizontalmente con el tiempo y reaparece del otro lado al
// salir del rango visible.

#include <memory>
#include <vector>
#include "ElementoEscena.h"
#include "ObjetoRenderizable.h"
#include "Shader.h"

class Nube : public ElementoEscena {
public:
    Nube(Vec3 posicionInicial, Shader* shader, float velocidadDeriva, unsigned semilla);

    void actualizar(float tiempo, const Oceano& oceano) override;
    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    static constexpr float LIMITE_DERIVA = 80.0f;

    Shader* shader_;
    std::unique_ptr<ObjetoRenderizable> mallaEsfera_;
    std::vector<Mat4> bultosLocales_;

    Vec3 posicionInicial_;
    Vec3 posicionActual_;
    float velocidadDeriva_;
};
#endif //CG_OCEANO_DINAMICO_NUBE_H