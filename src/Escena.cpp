#include "Escena.h"
#include "ElementoEscenaFactory.h"
#include "GestorOlas.h"
#include "Faro.h"

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
