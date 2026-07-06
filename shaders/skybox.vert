#version 330 core
// skybox.vert
// Cupula/hemisferio grande que envuelve la escena. No necesita normales
// ni iluminacion: el color se resuelve por gradiente segun la altura (y).

layout (location = 0) in vec3 aPos;

uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

out float alturaRelativa; // 0 en el horizonte, 1 en la cima de la cupula

void main() {
    vec4 posMundo = modelo * vec4(aPos, 1.0);
    // aPos ya viene normalizado (esfera unitaria) antes de escalar en 'modelo',
    // asi que aPos.y en [0,1] nos sirve directo como parametro del gradiente.
    alturaRelativa = clamp(aPos.y, 0.0, 1.0);
    gl_Position = proyeccion * vista * posMundo;
}
