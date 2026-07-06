#version 330 core
// basico.vert
// Shader Phong estandar (sin deformacion de olas) para los elementos
// adicionales de la escena: barco, isla/rocas, faro.

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

out vec3 fragPos;
out vec3 normal;

void main() {
    vec4 posMundo = modelo * vec4(aPos, 1.0);
    fragPos = posMundo.xyz;
    // NOTA: mat3(modelo) es correcto para rotacion + escala UNIFORME + traslacion.
    // Si mas adelante usas escala no-uniforme en algun elemento, cambiar por la
    // transpuesta-inversa de modelo para que las normales no se deformen.
    normal = normalize(mat3(modelo) * aNormal);
    gl_Position = proyeccion * vista * posMundo;
}
