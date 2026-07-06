#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string Shader::leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "[Shader] No se pudo abrir: " << ruta << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

unsigned int Shader::compilar(const std::string& fuente, GLenum tipo, const std::string& etiqueta) {
    unsigned int id = glCreateShader(tipo);
    const char* fuenteC = fuente.c_str();
    glShaderSource(id, 1, &fuenteC, nullptr);
    glCompileShader(id);

    int exito;
    glGetShaderiv(id, GL_COMPILE_STATUS, &exito);
    if (!exito) {
        char info[1024];
        glGetShaderInfoLog(id, 1024, nullptr, info);
        std::cerr << "[Shader] Error compilando " << etiqueta << ":\n" << info << "\n";
    }
    return id;
}

Shader::Shader(const std::string& rutaVertex, const std::string& rutaFragment) {
    std::string fuenteVertex = leerArchivo(rutaVertex);
    std::string fuenteFragment = leerArchivo(rutaFragment);

    unsigned int vertexId = compilar(fuenteVertex, GL_VERTEX_SHADER, rutaVertex);
    unsigned int fragmentId = compilar(fuenteFragment, GL_FRAGMENT_SHADER, rutaFragment);

    programaId_ = glCreateProgram();
    glAttachShader(programaId_, vertexId);
    glAttachShader(programaId_, fragmentId);
    glLinkProgram(programaId_);

    int exito;
    glGetProgramiv(programaId_, GL_LINK_STATUS, &exito);
    if (!exito) {
        char info[1024];
        glGetProgramInfoLog(programaId_, 1024, nullptr, info);
        std::cerr << "[Shader] Error enlazando (" << rutaVertex << " + " << rutaFragment << "):\n" << info << "\n";
    }

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
}

Shader::~Shader() {
    if (programaId_ != 0) glDeleteProgram(programaId_);
}

void Shader::usar() const { glUseProgram(programaId_); }

void Shader::setMat4(const std::string& nombre, const Mat4& valor) const {
    glUniformMatrix4fv(glGetUniformLocation(programaId_, nombre.c_str()), 1, GL_FALSE, valor.m.data());
}

void Shader::setVec3(const std::string& nombre, const Vec3& valor) const {
    glUniform3f(glGetUniformLocation(programaId_, nombre.c_str()), valor.x, valor.y, valor.z);
}

void Shader::setFloat(const std::string& nombre, float valor) const {
    glUniform1f(glGetUniformLocation(programaId_, nombre.c_str()), valor);
}

void Shader::setInt(const std::string& nombre, int valor) const {
    glUniform1i(glGetUniformLocation(programaId_, nombre.c_str()), valor);
}

void Shader::setBool(const std::string& nombre, bool valor) const {
    glUniform1i(glGetUniformLocation(programaId_, nombre.c_str()), valor ? 1 : 0);
}

void Shader::setFloatArray(const std::string& nombre, const float* valores, int cantidad) const {
    glUniform1fv(glGetUniformLocation(programaId_, nombre.c_str()), cantidad, valores);
}
