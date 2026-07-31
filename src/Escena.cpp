#include "Escena.h"
#include "ElementoEscenaFactory.h"
#include "GestorOlas.h"
#include "Faro.h"
#include "Barco.h"
#include <algorithm>
#include <cmath>
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
    // Dos barcos iniciales con orbitas que se CRUZAN a proposito (centros a
    // distancia 10, radios 6 cada uno -> se solapan) para poder ver una
    // colision sin tener que esperar al spawn aleatorio.
    elementos_.push_back(ElementoEscenaFactory::crear(
        TipoElemento::Barco, shaderBasico_.get(), Vec3{-8.0f, 0.0f, -5.0f}, /*radio*/ 6.0f, /*velocidad*/ 0.15f));
    elementos_.push_back(ElementoEscenaFactory::crear(
        TipoElemento::Barco, shaderBasico_.get(), Vec3{-8.0f, 0.0f, 5.0f}, /*radio*/ 6.0f, /*velocidad*/ -0.18f));

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
        float amplitud = amplitudErraticaAleatoria_(generadorErratica_);
        float frecuencia = frecuenciaErraticaAleatoria_(generadorErratica_);

        oceano_->activarOlaErratica(amplitud, direccionErratica_(generadorErratica_), frecuencia, /*duracion*/ 6.0f);

        std::cout << "[Escena] Ola erratica: amplitud=" << amplitud << " frecuencia=" << frecuencia
                  << (amplitud * (4.0f * 3.14159265f * 3.14159265f * frecuencia * frecuencia / 9.81f) > std::tan(0.70f)
                          ? "  -> deberia volcar" : "  -> solo tambaleo fuerte") << "\n";

        tiempoParaProximaErratica_ = intervaloErratica_(generadorErratica_);
    }

    // Todo lo de abajo toca elementos_ (agregar/quitar), por eso va DESPUES
    // del for de arriba y en este orden: primero resolver colisiones con las
    // posiciones ya actualizadas de este frame, luego sacar los que terminaron
    // de hundirse, y recien ahi ver si hay cupo para que aparezca uno nuevo.
    gestionarColisionesBarcos();
    eliminarBarcosHundidos();
    gestionarSpawnBarcos(deltaTiempo);
}

void Escena::gestionarColisionesBarcos() {
    std::vector<Barco*> barcos;
    for (auto& elemento : elementos_) {
        if (auto* barco = dynamic_cast<Barco*>(elemento.get())) barcos.push_back(barco);
    }

    for (size_t i = 0; i < barcos.size(); ++i) {
        for (size_t j = i + 1; j < barcos.size(); ++j) {
            Barco* a = barcos[i];
            Barco* b = barcos[j];
            if (a->enCooldownColision() || b->enCooldownColision()) continue;

            Vec3 diff = a->posicion() - b->posicion();
            float distancia = std::sqrt(diff.x * diff.x + diff.z * diff.z);
            float distanciaMinima = a->radioColision() + b->radioColision();
            if (distancia >= distanciaMinima) continue;

            // Impacto = suma de las rapideces tangenciales (radio*velocidadAngular)
            // de ambos - una aproximacion simple de que tan "fuerte" fue el choque.
            float impacto = std::abs(a->velocidadLineal()) + std::abs(b->velocidadLineal());

            if (impacto > UMBRAL_IMPACTO_ROMPE) {
                a->romper();
                b->romper();
                std::cout << "[Escena] Colision fuerte (impacto=" << impacto << ") - ambos barcos se rompen\n";
            } else {
                a->invertirDireccion();
                b->invertirDireccion();
            }

            a->marcarCooldownColision();
            b->marcarCooldownColision();
        }
    }
}

void Escena::eliminarBarcosHundidos() {
    elementos_.erase(
        std::remove_if(elementos_.begin(), elementos_.end(), [](const std::unique_ptr<ElementoEscena>& elemento) {
            auto* barco = dynamic_cast<Barco*>(elemento.get());
            return barco && barco->listoParaEliminar();
        }),
        elementos_.end()
    );
}

void Escena::gestionarSpawnBarcos(float deltaTiempo) {
    int barcosVivos = 0;
    for (auto& elemento : elementos_) {
        if (dynamic_cast<Barco*>(elemento.get())) ++barcosVivos;
    }
    if (barcosVivos >= MAX_BARCOS) return;

    tiempoParaProximoBarco_ -= deltaTiempo;
    if (tiempoParaProximoBarco_ > 0.0f) return;

    Vec3 centro{posicionBarcoAleatoria_(generadorErratica_), 0.0f, posicionBarcoAleatoria_(generadorErratica_)};
    float radio = radioBarcoAleatorio_(generadorErratica_);
    float velocidad = velocidadBarcoAleatoria_(generadorErratica_)
                     * (signoGiroAleatorio_(generadorErratica_) ? 1.0f : -1.0f);
    float anguloInicial = anguloBarcoAleatorio_(generadorErratica_);

    elementos_.push_back(ElementoEscenaFactory::crear(
        TipoElemento::Barco, shaderBasico_.get(), centro, radio, velocidad, anguloInicial));

    std::cout << "[Escena] Nuevo barco aparecio en (" << centro.x << ", " << centro.z
              << ") - total vivos: " << (barcosVivos + 1) << "/" << MAX_BARCOS << "\n";

    tiempoParaProximoBarco_ = intervaloBarcoAleatorio_(generadorErratica_);
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