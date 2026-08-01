#include "Escena.h"
#include "ElementoEscenaFactory.h"
#include "GestorOlas.h"
#include "Faro.h"
#include "Barco.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Pez.h"
#include "Sol.h"
#include "Luna.h"
#include "Nube.h"
#include "CicloDiaNoche.h"
#include <fstream>
#include "OBJLoader.h"

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
    shaderTexturado_ = std::make_unique<Shader>("shaders/textura_objeto.vert", "shaders/textura_objeto.frag");

    // Modelos pesados: se cargan UNA sola vez y se comparten entre todas las
    // instancias (2 barcos, 12 peces) - antes cada instancia los cargaba y
    // subia a GPU por separado, lo que probablemente agotaba la VRAM.
    modeloBarcoCompartido_ = cargarModeloCompartido(
        "models/barco_medieval/barco.obj", "models/barco_medieval/casco.jpg",
        "MED-BOAT", /*tamanoObjetivo*/ 6.5f, "Barco");

    modeloPezCompartido_ = cargarModeloCompartido(
        "models/pescado/pez.obj", "models/pescado/fish.jpg",
        "", /*tamanoObjetivo*/ 1.2f, "Pez");

    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Skybox, nullptr));
    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Isla, shaderBasico_.get(), Vec3{15.0f, 0.0f, 10.0f}));
    elementos_.push_back(ElementoEscenaFactory::crear(TipoElemento::Faro, shaderBasico_.get(), Vec3{15.0f, 1.0f, 10.0f}));
    // Dos barcos iniciales con orbitas que se CRUZAN a proposito (centros a
    // distancia 10, radios 6 cada uno -> se solapan) para poder ver una
    // colision sin tener que esperar al spawn aleatorio.
    elementos_.push_back(std::make_unique<Barco>(
    Vec3{-8.0f, 0.0f, -5.0f}, shaderBasico_.get(), shaderTexturado_.get(), modeloBarcoCompartido_.get(),
    /*radio*/ 6.0f, /*velocidad*/ 0.15f));
    elementos_.push_back(std::make_unique<Barco>(
        Vec3{-8.0f, 0.0f, 5.0f}, shaderBasico_.get(), shaderTexturado_.get(), modeloBarcoCompartido_.get(),
        /*radio*/ 6.0f, /*velocidad*/ -0.18f));

    elementos_.push_back(std::make_unique<Sol>(shaderTexturado_.get()));
    elementos_.push_back(std::make_unique<Luna>(shaderTexturado_.get()));
    //elementos_.push_back(std::make_unique<CicloDiaNoche>());

    crearGruposPeces();
    crearNubes();

    for (auto& elemento : elementos_) {
        if (auto* faro = dynamic_cast<Faro*>(elemento.get())) faro_ = faro;
    }

    crearGruposPeces();
    crearNubes();

    oceano_->establecerZonaCalma(posicionIsla_, radioColisionIsla_, /*margen*/ 6.0f);
}

void Escena::actualizar(float deltaTiempo) {
    tiempoTotal_ += deltaTiempo;
    oceano_->actualizar(deltaTiempo);

    // Luz principal sigue al sol de dia, a la luna de noche - misma formula
    // que usan Sol/Luna internamente, para quedar sincronizados.
    float f = CicloDiaNoche::factorDia(tiempoTotal_);
    float anguloSol = CicloDiaNoche::anguloSol(tiempoTotal_);
    float anguloLuna = CicloDiaNoche::anguloLuna(tiempoTotal_);

    Vec3 posSol{CicloDiaNoche::RADIO_ARCO_CIELO * std::cos(anguloSol), CicloDiaNoche::RADIO_ARCO_CIELO * std::sin(anguloSol), 0.0f};
    Vec3 posLuna{CicloDiaNoche::RADIO_ARCO_CIELO * std::cos(anguloLuna), CicloDiaNoche::RADIO_ARCO_CIELO * std::sin(anguloLuna), 0.0f};
    posLuzPrincipal_ = (f > 0.5f) ? posSol : posLuna;

    Vec3 colorDia{1.0f, 0.98f, 0.9f};
    Vec3 colorNoche{0.18f, 0.22f, 0.35f};
    colorLuzPrincipal_ = colorNoche + (colorDia - colorNoche) * f;

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
    gestionarColisionBarcoIsla();
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

    elementos_.push_back(std::make_unique<Barco>(
    centro, shaderBasico_.get(), shaderTexturado_.get(), modeloBarcoCompartido_.get(),
    radio, velocidad, anguloInicial));

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

void Escena::gestionarColisionBarcoIsla() {
    for (auto& elemento : elementos_) {
        auto* barco = dynamic_cast<Barco*>(elemento.get());
        if (!barco || barco->enCooldownColision()) continue;

        Vec3 diff = barco->posicion() - posicionIsla_;
        float distancia = std::sqrt(diff.x * diff.x + diff.z * diff.z);
        if (distancia < radioColisionIsla_ + barco->radioColision()) {
            barco->invertirDireccion();
            barco->marcarCooldownColision();
        }
    }
}

void Escena::crearGruposPeces() {
    // Tres zonas separadas, 4 peces sueltos cada una -> "varios grupos
    // pequenos en distintas zonas" en vez de un solo cardumen grande.
    std::vector<Vec3> zonas = { {-20.0f, 0.0f, -20.0f}, {20.0f, 0.0f, -15.0f}, {0.0f, 0.0f, 20.0f} };
    const int pecesPorGrupo = 4;

    for (const auto& zona : zonas) {
        for (int i = 0; i < pecesPorGrupo; ++i) {
            float profundidad = profundidadPezAleatoria_(generadorErratica_);
            float radio = radioPezAleatorio_(generadorErratica_);
            float velocidad = velocidadPezAleatoria_(generadorErratica_)
                             * (signoGiroAleatorio_(generadorErratica_) ? 1.0f : -1.0f);
            float angulo = anguloPezAleatorio_(generadorErratica_);
            elementos_.push_back(std::make_unique<Pez>(zona, profundidad, radio, velocidad, angulo,
                                             shaderTexturado_.get(), modeloPezCompartido_.get()));
        }
    }
}

void Escena::crearNubes() {
    std::uniform_real_distribution<float> posXAleatoria(-60.0f, 60.0f);
    std::uniform_real_distribution<float> posZAleatoria(-60.0f, 60.0f);
    std::uniform_real_distribution<float> alturaAleatoria(35.0f, 55.0f);
    std::uniform_real_distribution<float> velocidadAleatoria(0.5f, 1.5f);
    std::uniform_int_distribution<unsigned> semillaAleatoria(1u, 100000u);

    const int cantidadNubes = 6;
    for (int i = 0; i < cantidadNubes; ++i) {
        Vec3 posicion{posXAleatoria(generadorErratica_), alturaAleatoria(generadorErratica_), posZAleatoria(generadorErratica_)};
        float velocidad = velocidadAleatoria(generadorErratica_);
        elementos_.push_back(std::make_unique<Nube>(posicion, shaderBasico_.get(), velocidad, semillaAleatoria(generadorErratica_)));
    }
}
std::unique_ptr<ObjetoRenderizableTexturado> Escena::cargarModeloCompartido(
    const std::string& rutaObj, const std::string& rutaTextura,
    const std::string& grupo, float tamanoObjetivo, const std::string& etiquetaLog) {

    std::ifstream prueba(rutaObj);
    if (!prueba.good()) {
        std::cout << "[" << etiquetaLog << "] No se encontro " << rutaObj << "\n";
        return nullptr;
    }

    MallaOBJ malla = OBJLoader::cargar(rutaObj, grupo);
    if (malla.vertices.empty()) {
        std::cerr << "[" << etiquetaLog << "] " << rutaObj << " no genero triangulos.\n";
        return nullptr;
    }

    OBJLoader::normalizarEscala(malla, tamanoObjetivo);
    return std::make_unique<ObjetoRenderizableTexturado>(malla, rutaTextura);
}