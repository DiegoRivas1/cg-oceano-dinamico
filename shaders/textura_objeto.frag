#version 330 core
// textura_objeto.frag
// Phong estandar (igual que basico.frag) pero el color base sale de una
// textura muestreada por UV, no de un uniform de color solido.

in vec3 fragPos;
in vec3 normal;
in vec2 uv;

out vec4 colorFinal;

uniform sampler2D texturaObjeto;

uniform vec3 posLuz;
uniform vec3 colorLuz;
uniform vec3 posCamara;

uniform vec3 colorAmbiente;
uniform vec3 colorEspecular;
uniform float brillo;

void main() {
    vec3 colorBase = texture(texturaObjeto, uv).rgb;

    vec3 n = normalize(normal);
    vec3 dirLuz = normalize(posLuz - fragPos);
    vec3 dirVista = normalize(posCamara - fragPos);
    vec3 reflejo = reflect(-dirLuz, n);

    vec3 ambiente = colorAmbiente * colorBase;

    float difIntensidad = max(dot(n, dirLuz), 0.0);
    vec3 difuso = difIntensidad * colorBase * colorLuz;

    float espIntensidad = pow(max(dot(dirVista, reflejo), 0.0), brillo);
    vec3 especular = espIntensidad * colorEspecular * colorLuz;

    colorFinal = vec4(ambiente + difuso + especular, 1.0);
}
