#version 330 core
// textura_objeto.vert
// Igual que basico.vert pero con UV, para objetos con textura real
// (por ahora, el modelo .obj del barco).

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelo;
uniform mat4 vista;
uniform mat4 proyeccion;

out vec3 fragPos;
out vec3 normal;
out vec2 uv;

void main() {
    vec4 posMundo = modelo * vec4(aPos, 1.0);
    fragPos = posMundo.xyz;
    normal = normalize(mat3(modelo) * aNormal); // valido para escala uniforme, ver nota en basico.vert
    uv = aUV;
    gl_Position = proyeccion * vista * posMundo;
}
