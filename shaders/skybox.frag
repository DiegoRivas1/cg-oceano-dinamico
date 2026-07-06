#version 330 core
in float alturaRelativa;
out vec4 colorFinal;

uniform vec3 colorCima;      // celeste intenso
uniform vec3 colorHorizonte; // blanco/naranja tenue

void main() {
    vec3 color = mix(colorHorizonte, colorCima, alturaRelativa);
    colorFinal = vec4(color, 1.0);
}
