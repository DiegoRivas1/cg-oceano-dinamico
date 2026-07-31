#include "Barco.h"
#include "Oceano.h"
#include "OBJLoader.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

namespace {
constexpr const char* RUTA_MODELO_OBJ = "models/barco_medieval/barco.obj";
constexpr const char* RUTA_TEXTURA_CASCO = "models/barco_medieval/casco.jpg";
constexpr const char* NOMBRE_GRUPO_BARCO = "MED-BOAT"; // el .obj trae tambien un "Plane" que NO queremos
constexpr float PI = 3.14159265f;

// Normaliza una diferencia de angulos al rango (-PI, PI], para que la
// interpolacion siempre gire por el camino corto (nunca da una vuelta
// completa de mas al cruzar el limite 0/2*PI).
float normalizarDiferenciaAngular(float diferencia) {
    while (diferencia > PI) diferencia -= 2.0f * PI;
    while (diferencia < -PI) diferencia += 2.0f * PI;
    return diferencia;
}
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
    // actualizar() solo recibe tiempo ABSOLUTO (tiempoTotal_ de Escena), asi
    // que derivamos delta comparando con la llamada anterior. Necesario para
    // los timers de la maquina de estados (Volcado/Hundiendose/cooldown).
    float deltaTiempo = (tiempoAnteriorAbsoluto_ < 0.0f) ? 0.0f : (tiempo - tiempoAnteriorAbsoluto_);
    tiempoAnteriorAbsoluto_ = tiempo;

    if (cooldownColision_ > 0.0f) cooldownColision_ = std::max(0.0f, cooldownColision_ - deltaTiempo);

    if (estado_ == EstadoBarco::Hundiendose) {
        tiempoEnEstado_ += deltaTiempo;
        alturaActual_ -= deltaTiempo * 0.6f; // se hunde progresivamente, sin recalcular la orbita
        return;
    }

    if (estado_ == EstadoBarco::Volcado) {
        tiempoEnEstado_ += deltaTiempo;
        // Interpola el roll hacia ~180 grados, hacia el lado en que ya se
        // estaba inclinando (no cambia de lado a mitad de camino).
        float objetivoRoll = (roll_ >= 0.0f ? 1.0f : -1.0f) * PI;
        roll_ += (objetivoRoll - roll_) * std::min(1.0f, deltaTiempo * 2.0f);
        alturaActual_ = oceano.alturaEn(posicionBase_.x, posicionBase_.z); // sigue a la deriva sobre el agua

        if (tiempoEnEstado_ > DURACION_VOLCADO) {
            estado_ = EstadoBarco::Hundiendose;
            tiempoEnEstado_ = 0.0f;
        }
        return;
    }

    // --- Estado Normal: navegacion + flotacion de siempre ---

    // Navegacion: el barco recorre un circulo de radio radioOrbita_ alrededor
    // de centroOrbita_. anguloActual_ tambien nos da la tangente -> hacia
    // donde apunta la proa (yaw_).
    anguloActual_ = anguloInicial_ + tiempo * velocidadAngular_;
    posicionBase_.x = centroOrbita_.x + radioOrbita_ * std::cos(anguloActual_);
    posicionBase_.z = centroOrbita_.z + radioOrbita_ * std::sin(anguloActual_);

    // Tangente al circulo = direccion de movimiento. +-90 grados respecto al
    // radio, con el signo segun si la velocidad angular es horaria o antihoraria.
    float signoGiro = (velocidadAngular_ >= 0.0f) ? 1.0f : -1.0f;
    yawObjetivo_ = anguloActual_ - signoGiro * (PI * 0.5f);

    if (!yawInicializado_) {
        // Primer frame: no hay "giro previo" que animar, apuntamos directo.
        yaw_ = yawObjetivo_;
        yawInicializado_ = true;
    } else {
        // yaw_ PERSIGUE a yawObjetivo_ a velocidad limitada (como si el timon
        // tardara en girar), en vez de copiarlo directo - asi un cambio de
        // sentido (colision) se ve como un giro real, no un salto de 180°.
        float diferencia = normalizarDiferenciaAngular(yawObjetivo_ - yaw_);
        float pasoMax = VELOCIDAD_GIRO_MAX * deltaTiempo;
        if (std::abs(diferencia) <= pasoMax) {
            yaw_ = yawObjetivo_;
        } else {
            yaw_ += (diferencia > 0.0f ? pasoMax : -pasoMax);
        }
    }

    // Muestreamos la altura real del oceano bajo el barco para que flote
    // exactamente sobre la superficie deformada por las olas.
    alturaActual_ = oceano.alturaEn(posicionBase_.x, posicionBase_.z);

    // Pendiente local exacta (dh/dx, dh/dz) via derivada analitica -> el
    // barco se inclina como si "chocara" con la forma real del agua. Con
    // olas normales da un balanceo sutil; si el Oceano tiene una ola
    // erratica activa, la pendiente se vuelve mucho mas pronunciada.
    float dHdx = 0.0f, dHdz = 0.0f;
    oceano.gradienteEn(posicionBase_.x, posicionBase_.z, dHdx, dHdz);

    roll_  = std::atan(dHdx);
    pitch_ = std::atan(dHdz);

    // Si la pendiente es demasiado pronunciada (ola erratica pasando por
    // aqui), el barco se vuelca de verdad en vez de solo tambalear.
    if (std::abs(roll_) > UMBRAL_VOLCADO || std::abs(pitch_) > UMBRAL_VOLCADO) {
        estado_ = EstadoBarco::Volcado;
        tiempoEnEstado_ = 0.0f;
    }
}

void Barco::invertirDireccion() {
    velocidadAngular_ = -velocidadAngular_;
    // Reancla anguloInicial_ para que no haya un "salto" visual de posicion
    // en el frame del cambio de sentido (el angulo actual pasa a ser el
    // nuevo punto de partida con la velocidad ya invertida).
    anguloInicial_ = anguloActual_ - tiempoAnteriorAbsoluto_ * velocidadAngular_;
}

void Barco::romper() {
    if (estado_ == EstadoBarco::Normal) {
        estado_ = EstadoBarco::Hundiendose;
        tiempoEnEstado_ = 0.0f;
    }
}

bool Barco::listoParaEliminar() const {
    return estado_ == EstadoBarco::Hundiendose && tiempoEnEstado_ > DURACION_HUNDIMIENTO;
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