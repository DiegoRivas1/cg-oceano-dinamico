#pragma once
// Shader.h
// Wrapper simple para compilar/enlazar un par vertex+fragment shader
// y subir uniforms comodamente. Necesario porque pasamos de immediate
// mode (labs anteriores) a Core Profile con pipeline programable.

#include <glad/glad.h>
#include <string>
#include "Matematica.h"

class Shader {
public:
    Shader(const std::string& rutaVertex, const std::string& rutaFragment);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void usar() const;

    void setMat4(const std::string& nombre, const Mat4& valor) const;
    void setVec3(const std::string& nombre, const Vec3& valor) const;
    void setFloat(const std::string& nombre, float valor) const;
    void setInt(const std::string& nombre, int valor) const;
    void setBool(const std::string& nombre, bool valor) const;
    void setFloatArray(const std::string& nombre, const float* valores, int cantidad) const;

    unsigned int id() const { return programaId_; }

private:
    unsigned int programaId_ = 0;

    static std::string leerArchivo(const std::string& ruta);
    static unsigned int compilar(const std::string& fuente, GLenum tipo, const std::string& etiqueta);
};
