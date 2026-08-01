
#ifndef CG_OCEANO_DINAMICO_SOL_H
#define CG_OCEANO_DINAMICO_SOL_H
#include <memory>
#include "ElementoEscena.h"
#include "ObjetoRenderizableTexturado.h"
#include "Shader.h"

class Sol : public ElementoEscena {
public:
    explicit Sol(Shader* shaderTexturado);
    void actualizar(float tiempo, const Oceano& oceano) override;
    void dibujar(const Mat4& vista, const Mat4& proyeccion,
                 const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const override;

private:
    Shader* shaderTexturado_ = nullptr; // no-owning, compartido
    std::unique_ptr<ObjetoRenderizableTexturado> modelo_;
    bool modeloValido_ = false;
    Vec3 posicion_{0.0f, 0.0f, 0.0f};
};
#endif //CG_OCEANO_DINAMICO_SOL_H