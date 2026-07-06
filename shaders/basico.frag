#version 330 core
// basico.frag
// Phong estandar con color solido por objeto (sin textura).

in vec3 fragPos;
in vec3 normal;

out vec4 colorFinal;

uniform vec3 posLuz;
uniform vec3 colorLuz;
uniform vec3 posCamara;

uniform vec3 colorObjeto;
uniform vec3 colorAmbiente;
uniform vec3 colorEspecular;
uniform float brillo;

void main() {
    vec3 n = normalize(normal);
    vec3 dirLuz = normalize(posLuz - fragPos);
    vec3 dirVista = normalize(posCamara - fragPos);
    vec3 reflejo = reflect(-dirLuz, n);

    vec3 ambiente = colorAmbiente * colorObjeto;

    float difIntensidad = max(dot(n, dirLuz), 0.0);
    vec3 difuso = difIntensidad * colorObjeto * colorLuz;

    float espIntensidad = pow(max(dot(dirVista, reflejo), 0.0), brillo);
    vec3 especular = espIntensidad * colorEspecular * colorLuz;

    colorFinal = vec4(ambiente + difuso + especular, 1.0);
}
