#version 330 core
// oceano.vert
// Calcula h(x,z,t) = suma de N olas sinusoidales (formula del enunciado)
// y su normal ANALITICA a partir de las derivadas parciales de h,
// todo en GPU, una vez por vertice por frame.

layout (location = 0) in vec3 aPos;  // posicion base (x, 0, z)
layout (location = 1) in vec2 aUV;   // coordenadas de textura (s,t)

#define MAX_OLAS 32
uniform int   numOlas;
uniform float amplitud[MAX_OLAS];
uniform float direccion[MAX_OLAS];   // radianes
uniform float frecuencia[MAX_OLAS];
uniform float numeroOnda[MAX_OLAS];  // k = 4*pi^2*f^2 / 9.81 (precalculado en CPU)
uniform float fase[MAX_OLAS];

uniform float tiempo;
uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

out vec3 fragPos;
out vec3 normal;
out vec2 uv;

const float DOS_PI = 6.28318530718;

void main() {
    float x = aPos.x;
    float z = aPos.z;

    float altura = 0.0;
    float dHdx = 0.0;   // dh/dx, para la normal
    float dHdz = 0.0;   // dh/dz, para la normal

    for (int i = 0; i < numOlas; ++i) {
        float dirX = cos(direccion[i]);
        float dirZ = sin(direccion[i]);
        float k = numeroOnda[i];

        float faseTotal = k * (x * dirX + z * dirZ) - DOS_PI * frecuencia[i] * tiempo + fase[i];

        altura += amplitud[i] * cos(faseTotal);

        // d/dx[A*cos(k*(x*dx+z*dz) - w*t + p)] = -A*k*dx*sin(...)
        float derivComun = -amplitud[i] * k * sin(faseTotal);
        dHdx += derivComun * dirX;
        dHdz += derivComun * dirZ;
    }

    vec3 posicion = vec3(x, altura, z);

    // Superficie parametrizada como (x, h(x,z), z):
    // tangente_x = (1, dh/dx, 0), tangente_z = (0, dh/dz, 1)
    // normal = normalize(cruz(tangente_z, tangente_x)) = normalize(-dh/dx, 1, -dh/dz)
    normal = normalize(vec3(-dHdx, 1.0, -dHdz));

    vec4 posMundo = modelo * vec4(posicion, 1.0);
    fragPos = posMundo.xyz;
    uv = aUV;

    gl_Position = proyeccion * vista * posMundo;
}
