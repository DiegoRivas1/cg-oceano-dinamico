#pragma once
// ObjetoRenderizable.h
// Helper de COMPOSICION (no herencia) reutilizado por Barco/Isla/Faro:
// sube una MallaSimple a un VAO/VBO/EBO propio y sabe dibujarse a si
// mismo con el shader basico, dada una matriz modelo y un color.
// Evita repetir el codigo de OpenGL en cada elemento de la escena.

#include <memory>
#include "Primitivas.h"
#include "Shader.h"
#include "Matematica.h"

class ObjetoRenderizable {
public:
    explicit ObjetoRenderizable(const MallaSimple& malla);
    ~ObjetoRenderizable();

    ObjetoRenderizable(const ObjetoRenderizable&) = delete;
    ObjetoRenderizable& operator=(const ObjetoRenderizable&) = delete;

    void dibujar(const Shader& shader, const Mat4& modelo) const;

private:
    unsigned int vao_ = 0, vbo_ = 0, ebo_ = 0;
    size_t numIndices_ = 0;
};
