#pragma once
// PuntoMalla.h
// Como la altura y la normal se calculan en el vertex shader (GPU) en
// cada frame, la malla en CPU solo necesita guardar la posicion BASE
// (x, 0, z) y las coordenadas de textura. No hace falta nx,ny,nz aqui
// porque el shader las calcula analiticamente a partir de las derivadas
// de h(x,z,t).

struct PuntoMalla {
    float x, y, z;   // y siempre es 0 en la malla base
    float s, t;      // coordenadas de textura
};
