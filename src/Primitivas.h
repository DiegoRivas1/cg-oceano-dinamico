#pragma once
// Primitivas.h
// Generadores procedurales simples de malla, con posicion + normal por
// vertice, usados para construir Barco, Isla, Faro y Skybox sin depender
// de modelos .obj externos. Si prefieres reusar tu shared/Primitives.h
// y shared/OBJLoader.h de cg-labs, puedes reemplazar estas funciones por
// esas manteniendo la misma firma de MallaSimple.

#include <vector>

struct VerticeSimple {
    float x, y, z;
    float nx, ny, nz;
};

struct MallaSimple {
    std::vector<VerticeSimple> vertices;
    std::vector<unsigned int> indices;
};

namespace Primitivas {
    // Caja centrada en el origen, de dimensiones ancho x alto x profundidad.
    MallaSimple crearCaja(float ancho, float alto, float profundidad);

    // Esfera UV de radio 1 (escalar despues via matriz modelo).
    MallaSimple crearEsfera(int paralelos = 16, int meridianos = 24);

    // Cono con base en y=0 y punta en y=altura.
    MallaSimple crearCono(float radioBase, float altura, int segmentos = 20);

    // Cilindro con base en y=0 y tapa en y=altura.
    MallaSimple crearCilindro(float radio, float altura, int segmentos = 20);
}
