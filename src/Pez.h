
#ifndef CG_OCEANO_DINAMICO_PEZ_H
#define CG_OCEANO_DINAMICO_PEZ_H
// Pez.h
// Pez individual que nada en un circulo bajo la superficie real del agua
// (Oceano::alturaEn menos una profundidad fija), con un pequeno ondulado
// vertical para que el nado se vea organico. Varios Pez con el mismo
// centroZona pero radio/velocidad/fase distintos forman un "cardumen"
// suelto - la logica de grupo vive en Escena::crearGruposPeces(), no aqui.

#include <memory>
#include "ElementoEscena.h"
#include "ObjetoRenderizableTexturado.h"
#include "Shader.h"

class Pez : public ElementoEscena {
public:
    Pez(Vec3 centroZona, float profundidad, float radioOrbita, float velocidadAngular, float anguloInicial,
    Shader* shaderTexturado, ObjetoRenderizableTexturado* modeloCompartido);

    void actualizar(float tiempo, const Oceano& oceano) override;
    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    Shader* shaderTexturado_ = nullptr;
    ObjetoRenderizableTexturado* modelo_ = nullptr;
    bool modeloValido_ = false;

    Vec3 centroZona_;
    float profundidad_;
    float radioOrbita_;
    float velocidadAngular_;
    float anguloInicial_;
    float anguloActual_ = 0.0f;
    float yaw_ = 0.0f;

    Vec3 posicionActual_{0.0f, 0.0f, 0.0f};
};
#endif //CG_OCEANO_DINAMICO_PEZ_H