#version 330 core
// oceano.frag
// Iluminacion Phong (ambiente + difusa + especular) + textura de agua
// con UV animadas en el tiempo para dar sensacion de flujo.

in vec3 fragPos;
in vec3 normal;
in vec2 uv;

out vec4 colorFinal;

uniform sampler2D texturaOceano;
uniform bool usarTextura;
uniform bool usarIluminacion;
uniform float tiempo;

uniform vec3 posLuz;
uniform vec3 colorLuz;
uniform vec3 posCamara;

uniform vec3 colorAmbiente;
uniform vec3 colorDifuso;
uniform vec3 colorEspecular;
uniform float brillo;

void main() {
    vec3 colorBase = colorDifuso;

    if (usarTextura) {
        vec2 uvAnimado = uv + vec2(tiempo * 0.02, tiempo * 0.015);
        colorBase = texture(texturaOceano, uvAnimado).rgb;
    }

    if (!usarIluminacion) {
        colorFinal = vec4(colorBase, 1.0);
        return;
    }

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
