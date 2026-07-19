#include "Escena.h"
#include "ElementoEscenaFactory.h"
#include "GestorOlas.h"
#include "Faro.h"
#include <iostream>

Escena::Escena() {
    // --- Oceano: Builder + Strategy (olas desde archivo) ---
    OlasDesdeArchivo fuenteOlas("data/spectrum.txt");
    oceano_ = Oceano::Builder()
        .conTamano(150, 150)
        .conSeparacion(0.6f)
        .conOlas(fuenteOlas)
        .conTextura("textures/ocean.tga")
        .conShaders("shaders/oceano.vert", "shaders/oceano.frag")
        .construir();

    // --- Elementos adicionales: Factory ---
    shaderBasico_ = std::make_unique<Shader>("shaders/basico.vert", "shaders/basico.frag");

    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Skybox, nullptr));
    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Isla, shaderBasico_.get(), Vec3{15.0f, 0.0f, 10.0f}));
    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Faro, shaderBasico_.get(), Vec3{15.0f, 1.0f, 10.0f}));
    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Barco, shaderBasico_.get(), Vec3{-8.0f, 0.0f, -5.0f}));

    for (auto& elemento : elementos_) {
        if (auto* faro = dynamic_cast<Faro*>(elemento.get())) faro_ = faro;
    }
}

void Escena::actualizar(float deltaTiempo) {
    tiempoTotal_ += deltaTiempo;
    oceano_->actualizar(deltaTiempo);
    for (auto& elemento : elementos_) elemento->actualizar(tiempoTotal_, *oceano_);

    // Ola erratica periodica: amplitud bien por encima de las olas normales
    // (que rondan 0.2-0.5, ver spectrum.txt) para que se note claramente.
    tiempoParaProximaErratica_ -= deltaTiempo;
    if (tiempoParaProximaErratica_ <= 0.0f) {
        oceano_->activarOlaErratica(
            /*amplitudMax*/ 3.0f,
            /*direccionRad*/ direccionErratica_(generadorErratica_),
            /*frecuencia*/ 0.18f,
            /*duracion*/ 6.0f
        );
        tiempoParaProximaErratica_ = intervaloErratica_(generadorErratica_);
    }

    // Barcos extra: aparicion aleatoria, uno a la vez, hasta MAX_BARCOS_EXTRA.
    // Se agregan DESPUES del for de arriba para no invalidar su iteracion.
    if (barcosExtraCreados_ < MAX_BARCOS_EXTRA) {
        tiempoParaProximoBarco_ -= deltaTiempo;
        if (tiempoParaProximoBarco_ <= 0.0f) {
            Vec3 centro{posicionBarcoAleatoria_(generadorErratica_), 0.0f, posicionBarcoAleatoria_(generadorErratica_)};
            float radio = radioBarcoAleatorio_(generadorErratica_);
            float velocidad = velocidadBarcoAleatoria_(generadorErratica_)
                             * (signoGiroAleatorio_(generadorErratica_) ? 1.0f : -1.0f);
            float anguloInicial = anguloBarcoAleatorio_(generadorErratica_);

            elementos_.push_back(ElementoEscenaFactory::crear(
                TipoElemento::Barco, shaderBasico_.get(), centro, radio, velocidad, anguloInicial));

            std::cout << "[Escena] Barco extra #" << (barcosExtraCreados_ + 1)
                      << " aparecio en (" << centro.x << ", " << centro.z << ")\n";

            ++barcosExtraCreados_;
            tiempoParaProximoBarco_ = intervaloBarcoAleatorio_(generadorErratica_);
        }
    }
}

void Escena::dibujar(float aspecto) const {
    Mat4 vista = camara_.obtenerVista();
    Mat4 proyeccion = Mat4::perspectiva(0.8f, aspecto, 0.1f, 500.0f);
    Vec3 posCamara = camara_.posicion();

    // NOTA: faro_->posicionLuz() expone una segunda fuente de luz calida,
    // disponible para quien extienda el shader basico a multiples luces;
    // por ahora el Phong usa la luz principal como dominante para mantener
    // el fragment shader simple.

    // Skybox primero (queda al fondo gracias a GL_LEQUAL + depthMask false)
    for (auto& elemento : elementos_) elemento->dibujar(vista, proyeccion, posCamara, posLuzPrincipal_, colorLuzPrincipal_);

    oceano_->dibujar(vista, proyeccion, posCamara, posLuzPrincipal_, colorLuzPrincipal_);
}