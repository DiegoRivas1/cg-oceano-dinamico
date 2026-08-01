

#ifndef CG_OCEANO_DINAMICO_CICLODIANOCHE_H
#define CG_OCEANO_DINAMICO_CICLODIANOCHE_H
// CicloDiaNoche.h
// Formula compartida del ciclo dia/noche (60s por defecto). Sol, Luna,
// Skybox y la luz principal de Escena derivan TODOS su estado de esta
// misma formula aplicada al mismo "tiempo" absoluto que ya reciben via
// ElementoEscena::actualizar(tiempo, oceano) - asi quedan sincronizados
// sin necesidad de que se consulten entre si.

#include <cmath>

namespace CicloDiaNoche {

    constexpr float DURACION_SEGUNDOS = 60.0f;
    constexpr float RADIO_ARCO_CIELO = 150.0f; // distancia del sol/luna al centro de la escena

    // Angulo del sol en su arco: 0 = amanecer, PI/2 = mediodia (zenit),
    // PI = atardecer, 3*PI/2 = medianoche (nadir, bajo el horizonte).
    inline float anguloSol(float tiempo) {
        float f = std::fmod(tiempo, DURACION_SEGUNDOS) / DURACION_SEGUNDOS;
        if (f < 0.0f) f += 1.0f;
        return f * 2.0f * 3.14159265f;
    }

    // La luna esta siempre en el angulo opuesto al sol: cuando uno sale, el otro se pone.
    inline float anguloLuna(float tiempo) {
        return anguloSol(tiempo) + 3.14159265f;
    }

    // 0.0 = noche cerrada, 1.0 = dia pleno, con transicion suave (smoothstep)
    // cerca del horizonte en vez de un corte brusco dia/noche.
    inline float factorDia(float tiempo) {
        float alturaSol = std::sin(anguloSol(tiempo));
        float t = (alturaSol - (-0.15f)) / (0.15f - (-0.15f));
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return t * t * (3.0f - 2.0f * t);
    }

} // namespace CicloDiaNoche
#endif //CG_OCEANO_DINAMICO_CICLODIANOCHE_H