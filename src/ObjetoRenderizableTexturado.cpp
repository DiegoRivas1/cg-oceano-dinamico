#include "ObjetoRenderizableTexturado.h"

#include <glad/glad.h>
#include <iostream>

// NOTA: NO se vuelve a poner #define STB_IMAGE_IMPLEMENTATION aqui - ya esta
// definido una sola vez en Oceano.cpp. Aqui solo se incluye la declaracion.
#include "stb_image.h"

ObjetoRenderizableTexturado::ObjetoRenderizableTexturado(const MallaOBJ& malla, const std::string& rutaTextura) {
    numVertices_ = malla.vertices.size();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, malla.vertices.size() * sizeof(VerticeOBJ), malla.vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VerticeOBJ), (void*)offsetof(VerticeOBJ, x));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VerticeOBJ), (void*)offsetof(VerticeOBJ, nx));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VerticeOBJ), (void*)offsetof(VerticeOBJ, u));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glGenTextures(1, &texturaId_);
    glBindTexture(GL_TEXTURE_2D, texturaId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int ancho, alto, canales;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* datos = stbi_load(rutaTextura.c_str(), &ancho, &alto, &canales, 0);
    if (datos) {
        GLenum formato;

        switch (canales) {
        case 1: formato = GL_RED;  break;
        case 3: formato = GL_RGB;  break;
        case 4: formato = GL_RGBA; break;
        default:
            std::cerr << "Formato de textura no soportado\n";
            stbi_image_free(datos);
            return;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, formato,
                     ancho, alto, 0,
                     formato, GL_UNSIGNED_BYTE, datos);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "[ObjetoRenderizableTexturado] Textura cargada: " << rutaTextura << "\n";
    } else {
        std::cerr << "[ObjetoRenderizableTexturado] No se pudo cargar textura: " << rutaTextura << "\n";
    }
    stbi_image_free(datos);
    glBindTexture(GL_TEXTURE_2D, 0);
}

ObjetoRenderizableTexturado::~ObjetoRenderizableTexturado() {
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (texturaId_) glDeleteTextures(1, &texturaId_);
}

void ObjetoRenderizableTexturado::dibujar(const Shader& shader, const Mat4& modelo) const {
    shader.setMat4("modelo", modelo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texturaId_);
    shader.setInt("texturaObjeto", 0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(numVertices_));
    glBindVertexArray(0);
}