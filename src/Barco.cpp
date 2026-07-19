#include "Barco.h"
#include "Oceano.h"
#include "OBJLoader.h"
#include <cmath>
#include <fstream>
#include <iostream>

namespace {
constexpr const char* RUTA_MODELO_OBJ = "models/barco_medieval/barco.obj";
constexpr const char* RUTA_TEXTURA_CASCO = "models/barco_medieval/casco.jpg";
constexpr const char* NOMBRE_GRUPO_BARCO = "MED-BOAT"; // el .obj trae tambien un "Plane" que NO queremos
}

Barco::Barco(Vec3 centroOrbita, Shader* shader, float radioOrbita, float velocidadAngular, float anguloInicial)
    : shader_(shader),
      centroOrbita_(centroOrbita),
      radioOrbita_(radioOrbita),
      velocidadAngular_(velocidadAngular),
      anguloInicial_(anguloInicial) {

    std::ifstream prueba(RUTA_MODELO_OBJ);
    usaModeloReal_ = prueba.good();

    if (usaModeloReal_) {
        std::cout << "[Barco] Modelo real encontrado, cargando " << RUTA_MODELO_OBJ << "\n";
        MallaOBJ malla = OBJLoader::cargar(RUTA_MODELO_OBJ, NOMBRE_GRUPO_BARCO);

        if (malla.vertices.empty()) {
            // El nombre del grupo no coincidio (o el .obj cambio) - no hay
            // triangulos que dibujar. Nos caemos al fallback procedural en
            // vez de mostrar un barco invisible.
            std::cerr << "[Barco] El grupo '" << NOMBRE_GRUPO_BARCO << "' no dio triangulos, uso fallback procedural.\n";
            usaModeloReal_ = false;
        } else {
            OBJLoader::normalizarEscala(malla, /*tamanoObjetivo*/ 4.0f);
            shaderTexturado_ = std::make_unique<Shader>("shaders/textura_objeto.vert", "shaders/textura_objeto.frag");
            modeloReal_ = std::make_unique<ObjetoRenderizableTexturado>(malla, RUTA_TEXTURA_CASCO);
        }
    } else {
        std::cout << "[Barco] No se encontro " << RUTA_MODELO_OBJ << ", uso primitivas procedurales.\n";
    }

    if (!usaModeloReal_) {
        casco_  = std::make_unique<ObjetoRenderizable>(Primitivas::crearCaja(2.5f, 0.8f, 1.0f));
        mastil_ = std::make_unique<ObjetoRenderizable>(Primitivas::crearCilindro(0.06f, 2.2f));
        vela_   = std::make_unique<ObjetoRenderizable>(Primitivas::crearCaja(0.05f, 1.4f, 0.9f));
    }
}

void Barco::actualizar(float tiempo, const Oceano& oceano) {
    // Navegacion: el barco recorre un circulo de radio radioOrbita_ alrededor
    // de centroOrbita_. anguloActual_ tambien nos da la tangente -> hacia
    // donde apunta la proa (yaw_).
    anguloActual_ = anguloInicial_ + tiempo * velocidadAngular_;
    posicionBase_.x = centroOrbita_.x + radioOrbita_ * std::cos(anguloActual_);
    posicionBase_.z = centroOrbita_.z + radioOrbita_ * std::sin(anguloActual_);

    // Tangente al circulo = direccion de movimiento. +90 grados respecto al
    // radio, con el signo segun si la velocidad angular es horaria o antihoraria.
    float signoGiro = (velocidadAngular_ >= 0.0f) ? 1.0f : -1.0f;
    yaw_ = anguloActual_ - signoGiro * (3.14159265f * 0.5f);

    // Muestreamos la altura real del oceano bajo el barco para que flote
    // exactamente sobre la superficie deformada por las olas.
    alturaActual_ = oceano.alturaEn(posicionBase_.x, posicionBase_.z);

    // Pendiente local exacta (dh/dx, dh/dz) via derivada analitica -> el
    // barco se inclina como si "chocara" con la forma real del agua. Con
    // olas normales da un balanceo sutil; si el Oceano tiene una ola
    // erratica activa, la pendiente se vuelve mucho mas pronunciada y el
    // barco se inclina de forma dramatica sin necesitar codigo especial.
    float dHdx = 0.0f, dHdz = 0.0f;
    oceano.gradienteEn(posicionBase_.x, posicionBase_.z, dHdx, dHdz);

    roll_  = std::atan(dHdx);
    pitch_ = std::atan(dHdz);
}

void Barco::dibujar(const Mat4& vista, const Mat4& proyeccion,
                     const Vec3& posCamara, const Vec3& posLuz, const Vec3& colorLuz) const {
    Mat4 flotacion = Mat4::trasladar({posicionBase_.x, alturaActual_ + 0.4f, posicionBase_.z})
                    * Mat4::rotarY(yaw_)
                    * Mat4::rotarZ(roll_)
                    * Mat4::rotarX(pitch_);

    if (usaModeloReal_) {
        shaderTexturado_->usar();
        shaderTexturado_->setMat4("vista", vista);
        shaderTexturado_->setMat4("proyeccion", proyeccion);
        shaderTexturado_->setVec3("posCamara", posCamara);
        shaderTexturado_->setVec3("posLuz", posLuz);
        shaderTexturado_->setVec3("colorLuz", colorLuz);
        shaderTexturado_->setVec3("colorAmbiente", Vec3{0.35f, 0.35f, 0.35f});
        shaderTexturado_->setVec3("colorEspecular", Vec3{0.2f, 0.2f, 0.2f});
        shaderTexturado_->setFloat("brillo", 12.0f);

        modeloReal_->dibujar(*shaderTexturado_, flotacion);
        return;
    }

    shader_->usar();
    shader_->setMat4("vista", vista);
    shader_->setMat4("proyeccion", proyeccion);
    shader_->setVec3("posCamara", posCamara);
    shader_->setVec3("posLuz", posLuz);
    shader_->setVec3("colorLuz", colorLuz);
    shader_->setVec3("colorAmbiente", Vec3{0.25f, 0.25f, 0.25f});
    shader_->setVec3("colorEspecular", Vec3{0.4f, 0.4f, 0.4f});
    shader_->setFloat("brillo", 16.0f);

    shader_->setVec3("colorObjeto", Vec3{0.35f, 0.22f, 0.10f}); // casco: madera
    casco_->dibujar(*shader_, flotacion);

    shader_->setVec3("colorObjeto", Vec3{0.30f, 0.20f, 0.10f});
    mastil_->dibujar(*shader_, flotacion * Mat4::trasladar({0.0f, 0.4f, 0.0f}));

    shader_->setVec3("colorObjeto", Vec3{0.95f, 0.95f, 0.90f}); // vela: blanco
    vela_->dibujar(*shader_, flotacion * Mat4::trasladar({0.0f, 0.9f, 0.0f}));
}