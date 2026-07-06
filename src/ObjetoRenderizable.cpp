#include "ObjetoRenderizable.h"
#include <glad/glad.h>

ObjetoRenderizable::ObjetoRenderizable(const MallaSimple& malla) {
    numIndices_ = malla.indices.size();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, malla.vertices.size() * sizeof(VerticeSimple), malla.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, malla.indices.size() * sizeof(unsigned int), malla.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VerticeSimple), (void*)offsetof(VerticeSimple, x));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VerticeSimple), (void*)offsetof(VerticeSimple, nx));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

ObjetoRenderizable::~ObjetoRenderizable() {
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

void ObjetoRenderizable::dibujar(const Shader& shader, const Mat4& modelo) const {
    shader.setMat4("modelo", modelo);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices_), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
