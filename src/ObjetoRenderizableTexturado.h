#ifndef CG_OCEANO_DINAMICO_OBJETORENDERIZABLETEXTURADO_H
#define CG_OCEANO_DINAMICO_OBJETORENDERIZABLETEXTURADO_H
//#pragma once
// ObjetoRenderizableTexturado.h
// Como ObjetoRenderizable, pero para mallas con UV (cargadas de un .obj) que
// se dibujan con una textura real en vez de un color solido. Se usa para el
// modelo del barco medieval; Isla/Faro siguen usando ObjetoRenderizable
// (color solido) porque no tienen textura propia.

#include <string>
#include "OBJLoader.h"
#include "Shader.h"
#include "Matematica.h"

class ObjetoRenderizableTexturado {
public:
    ObjetoRenderizableTexturado(const MallaOBJ& malla, const std::string& rutaTextura);
    ~ObjetoRenderizableTexturado();

    ObjetoRenderizableTexturado(const ObjetoRenderizableTexturado&) = delete;
    ObjetoRenderizableTexturado& operator=(const ObjetoRenderizableTexturado&) = delete;

    void dibujar(const Shader& shader, const Mat4& modelo) const;

private:
    unsigned int vao_ = 0, vbo_ = 0;
    unsigned int texturaId_ = 0;
    size_t numVertices_ = 0;
};
#endif //CG_OCEANO_DINAMICO_OBJETORENDERIZABLETEXTURADO_H