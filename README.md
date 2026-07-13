# cg-oceano-dinamico


Sistema de animación por computador de un océano en movimiento, usando OpenGL Core Profile + GLSL.

## Descripción

La superficie del océano es una malla regular de triángulos sobre el plano `xz`.
Cada punto sube y baja con el tiempo según la superposición de **19 olas
sinusoidales** (cargadas desde `data/spectrum.txt`), siguiendo la fórmula:

```
h(x, z, t) = Σ Aᵢ · cos( kᵢ·(x·cos(dᵢ) + z·sin(dᵢ)) − 2π·fᵢ·t + pᵢ )
kᵢ = 4π²fᵢ² / 9.81
```

A diferencia de los labs anteriores (immediate mode / CPU), aquí la altura y
la normal de cada vértice se calculan **en el vertex shader (GPU)**, una vez
por vértice por frame — más eficiente con 19 olas y una malla de ~150×150 puntos.

## Estructura de clases

| Clase | Responsabilidad |
|---|---|
| `Ola` | Una ola individual: amplitud, dirección, frecuencia, fase, número de onda. También evalúa `altura(x,z,t)` en CPU (usada por el barco para flotar). |
| `FuenteOlas` / `OlasManual` / `OlasDesdeArchivo` | **Patrón Strategy** origen de las olas (código vs. `spectrum.txt`), intercambiable sin tocar `Oceano`. |
| `PuntoMalla` | Vértice base de la malla: posición `(x,0,z)` + UV `(s,t)`. |
| `Oceano` | Construye la malla, sube uniforms de olas, dibuja. Se construye con **`Oceano::Builder`**. |
| `Shader` | Wrapper de compilación/enlace de programas GLSL y subida de uniforms. |
| `Camara` | Cámara orbital (rotar, zoom, `obtenerVista()`). |
| `Primitivas` | Generadores procedurales de malla (caja, esfera, cono, cilindro). |
| `ObjetoRenderizable` | Composición: sube una `MallaSimple` a GPU y se dibuja con un `Shader` dado. |
| `ElementoEscena` | Interfaz común de `Barco`, `Isla`, `Faro`, `Skybox`. |
| `ElementoEscenaFactory` | **Patrón Factory Method** crea cada tipo de elemento adicional. |
| `Escena` | Composición final: `Oceano` + elementos + `Camara` + luces. |

## Elementos adicionales de la escena

- **Barco** flota muestreando `Oceano::alturaEn(x,z)` bajo su posición, con balanceo según la pendiente local.
- **Isla + rocas** generación procedural con semilla fija (reproducible).
- **Faro** además decorativo, expone `posicionLuz()` como segunda fuente de luz (parpadeo incluido).
- **Skybox** cúpula con gradiente de color (sin depender de texturas cubemap externas).

## Compilar (CLion + MSYS2 UCRT64)

1. Instala GLFW vía pacman: `pacman -S mingw-w64-ucrt-x86_64-glfw`
2. Copia tus dependencias vendoreadas (ver `external/README.md` y `shared/README.md`):
   - `external/glad/`
   - `external/imgui-1.92.8/` (con backend **opengl3**, no opengl2)
   - `shared/stb_image.h`
3. Abre la carpeta en CLion con el toolchain MSYS2 UCRT64 configurado.
4. Build & Run del target `cg_oceano`.

El `CMakeLists.txt` copia automáticamente `shaders/`, `textures/` y `data/`
junto al ejecutable en cada build.

## Controles

| Acción | Control |
|---|---|
| Rotar cámara | Click izquierdo + arrastrar |
| Zoom | Scroll del mouse |
| Activar/desactivar textura | `T` |
| Activar/desactivar iluminación | `L` |
| Velocidad de animación | `+` / `-`, o el slider del panel ImGui |
| Salir | `Esc` |

## Archivo de olas (`data/spectrum.txt`)

3 columnas separadas por espacio/tab: `amplitud  dirección(rad)  frecuencia`.
La fase no está en el archivo se genera aleatoriamente con semilla fija
(`OlasDesdeArchivo`, semilla=42) para que la animación sea reproducible entre
ejecuciones (útil para grabar el video de entrega).

## Pendiente para la entrega

- [ ] Grabar video corto de la animación
- [ ] Completar `docs/informe/informe.tex` con capturas propias
- [ ] Revisar rendimiento con las 19 olas en tu hardware (bajar `filas/columnas` en `Escena.cpp` si hace falta)
- [ ] (Opcional) Reemplazar primitivas procedurales del barco/faro por modelos `.obj` si quieres más detalle visual
