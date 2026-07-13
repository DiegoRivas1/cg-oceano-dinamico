#include "Oceano.h"
#include "PuntoMalla.h"

#include <glad/glad.h>
#include <cmath>
#include <iostream>

// -------------------- Define STB solo en esta traduccion --------------------
// stb_image.h se copia desde tus otros labs (shared/) - es header-only.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ============================== Builder ==============================

Oceano::Builder& Oceano::Builder::conTamano(int filas, int columnas) {
    filas_ = filas;
    columnas_ = columnas;
    return *this;
}

Oceano::Builder& Oceano::Builder::conSeparacion(float separacion) {
    separacion_ = separacion;
    return *this;
}

Oceano::Builder& Oceano::Builder::conOlas(const FuenteOlas& fuente) {
    olas_ = fuente.obtenerOlas();
    return *this;
}

Oceano::Builder& Oceano::Builder::conTextura(const std::string& rutaTextura) {
    rutaTextura_ = rutaTextura;
    return *this;
}

Oceano::Builder& Oceano::Builder::conShaders(const std::string& rutaVertex, const std::string& rutaFragment) {
    rutaVertex_ = rutaVertex;
    rutaFragment_ = rutaFragment;
    return *this;
}

std::unique_ptr<Oceano> Oceano::Builder::construir() {
    auto oceano = std::unique_ptr<Oceano>(new Oceano());

    if (static_cast<int>(olas_.size()) > Oceano::MAX_OLAS) {
        std::cerr << "[Oceano] Advertencia: " << olas_.size()
                  << " olas superan MAX_OLAS=" << Oceano::MAX_OLAS
                  << ", se recortan las excedentes.\n";
        olas_.erase(olas_.begin() + Oceano::MAX_OLAS, olas_.end());
    }

    oceano->olas_ = olas_;
    oceano->shader_ = std::make_unique<Shader>(rutaVertex_, rutaFragment_);
    oceano->construirMalla(filas_, columnas_, separacion_);
    if (!rutaTextura_.empty()) oceano->cargarTextura(rutaTextura_);

    return oceano;
}

// ============================== Oceano ==============================

Oceano::~Oceano() {
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (texturaId_) glDeleteTextures(1, &texturaId_);
}

void Oceano::construirMalla(int filas, int columnas, float separacion) {
    filas_ = filas;
    columnas_ = columnas;
    separacion_ = separacion;

    std::vector<PuntoMalla> vertices;
    vertices.reserve(static_cast<size_t>(filas) * columnas);

    float anchoTotal = (columnas - 1) * separacion;
    float largoTotal = (filas - 1) * separacion;

    for (int fila = 0; fila < filas; ++fila) {
        for (int col = 0; col < columnas; ++col) {
            PuntoMalla p{};
            p.x = col * separacion - anchoTotal * 0.5f;
            p.y = 0.0f;
            p.z = fila * separacion - largoTotal * 0.5f;
            p.s = static_cast<float>(col) / (columnas - 1);
            p.t = static_cast<float>(fila) / (filas - 1);
            vertices.push_back(p);
        }
    }

    std::vector<unsigned int> indices;
    indices.reserve(static_cast<size_t>(filas - 1) * (columnas - 1) * 6);

    for (int fila = 0; fila < filas - 1; ++fila) {
        for (int col = 0; col < columnas - 1; ++col) {
            unsigned int actual = fila * columnas + col;
            unsigned int siguienteFila = (fila + 1) * columnas + col;

            indices.push_back(actual);
            indices.push_back(siguienteFila);
            indices.push_back(actual + 1);

            indices.push_back(actual + 1);
            indices.push_back(siguienteFila);
            indices.push_back(siguienteFila + 1);
        }
    }
    numIndices_ = indices.size();

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PuntoMalla), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // layout(location=0) posicion base (x,y=0,z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PuntoMalla), (void*)offsetof(PuntoMalla, x));
    glEnableVertexAttribArray(0);

    // layout(location=1) coordenadas de textura (s,t)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PuntoMalla), (void*)offsetof(PuntoMalla, s));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Oceano::cargarTextura(const std::string& rutaTextura) {
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
        GLenum formato = (canales == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, formato, ancho, alto, 0, formato, GL_UNSIGNED_BYTE, datos);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "[Oceano] Textura cargada: " << rutaTextura << " (" << ancho << "x" << alto << ")\n";
    } else {
        std::cerr << "[Oceano] No se pudo cargar la textura: " << rutaTextura << "\n";
    }
    stbi_image_free(datos);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Oceano::actualizar(float deltaTiempo) {
    tiempoAcumulado_ += deltaTiempo * velocidad_;
}

std::optional<Ola> Oceano::olaErraticaActual() const {
    if (!olaErratica_.activa) return std::nullopt;

    float t = tiempoAcumulado_ - olaErratica_.tiempoInicio;
    if (t < 0.0f || t > olaErratica_.duracion) return std::nullopt;

    // Envolvente en forma de campana: 0 al inicio, pico en el centro, 0 al final.
    float u = t / olaErratica_.duracion;
    float envolvente = std::sin(3.14159265f * u);
    float amplitud = olaErratica_.amplitudMax * envolvente;

    return Ola(amplitud, olaErratica_.direccion, olaErratica_.frecuencia, olaErratica_.fase);
}

void Oceano::activarOlaErratica(float amplitudMax, float direccionRad, float frecuencia, float duracion) {
    olaErratica_.activa = true;
    olaErratica_.amplitudMax = amplitudMax;
    olaErratica_.direccion = direccionRad;
    olaErratica_.frecuencia = frecuencia;
    olaErratica_.fase = 0.0f;
    olaErratica_.tiempoInicio = tiempoAcumulado_;
    olaErratica_.duracion = duracion;
}

void Oceano::gradienteEn(float x, float z, float& dHdx, float& dHdz) const {
    dHdx = 0.0f;
    dHdz = 0.0f;
    for (const auto& ola : olas_) ola.gradiente(x, z, tiempoAcumulado_, dHdx, dHdz);
    if (auto errante = olaErraticaActual()) errante->gradiente(x, z, tiempoAcumulado_, dHdx, dHdz);
}

void Oceano::subirUniformesOlas() const {
    float amplitudes[MAX_OLAS]{}, direcciones[MAX_OLAS]{}, frecuencias[MAX_OLAS]{};
    float numerosOnda[MAX_OLAS]{}, fases[MAX_OLAS]{};

    int n = static_cast<int>(olas_.size());
    for (int i = 0; i < n; ++i) {
        amplitudes[i]   = olas_[i].amplitud();
        direcciones[i]  = olas_[i].direccion();
        frecuencias[i]  = olas_[i].frecuencia();
        numerosOnda[i]  = olas_[i].numeroOnda();
        fases[i]        = olas_[i].fase();
    }

    // La ola erratica ocupa un slot extra mientras esta activa (si hay espacio).
    if (auto errante = olaErraticaActual(); errante && n < MAX_OLAS) {
        amplitudes[n]   = errante->amplitud();
        direcciones[n]  = errante->direccion();
        frecuencias[n]  = errante->frecuencia();
        numerosOnda[n]  = errante->numeroOnda();
        fases[n]        = errante->fase();
        ++n;
    }

    shader_->setInt("numOlas", n);
    shader_->setFloatArray("amplitud", amplitudes, n);
    shader_->setFloatArray("direccion", direcciones, n);
    shader_->setFloatArray("frecuencia", frecuencias, n);
    shader_->setFloatArray("numeroOnda", numerosOnda, n);
    shader_->setFloatArray("fase", fases, n);
}

void Oceano::dibujar(const Mat4& vista, const Mat4& proyeccion, const Vec3& posCamara,
                      const Vec3& posLuz, const Vec3& colorLuz) const {
    shader_->usar();

    shader_->setMat4("modelo", Mat4::identidad());
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setFloat("tiempo", tiempoAcumulado_);

    shader_->setBool("usarTextura", usarTextura_);
    shader_->setBool("usarIluminacion", usarIluminacion_);

    shader_->setVec3("posCamara", posCamara);
    shader_->setVec3("posLuz", posLuz);
    shader_->setVec3("colorLuz", colorLuz);

    shader_->setVec3("colorAmbiente", Vec3{0.15f, 0.2f, 0.25f});
    shader_->setVec3("colorDifuso", Vec3{0.05f, 0.25f, 0.35f});
    shader_->setVec3("colorEspecular", Vec3{1.0f, 1.0f, 1.0f});
    shader_->setFloat("brillo", 64.0f);

    subirUniformesOlas();

    if (usarTextura_) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaId_);
        shader_->setInt("texturaOceano", 0);
    }

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices_), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

float Oceano::alturaEn(float x, float z) const {
    float h = 0.0f;
    for (const auto& ola : olas_) h += ola.altura(x, z, tiempoAcumulado_);
    if (auto errante = olaErraticaActual()) h += errante->altura(x, z, tiempoAcumulado_);
    return h;
}
