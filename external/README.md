# external/

Esta carpeta no se versiona con el código pesado (o sí, según prefieras — muchos
cursos piden el proyecto autocontenido). Copia aquí las mismas dependencias que
ya usas en `cg-labs`:

```
external/
├── glad/
│   ├── include/glad/glad.h, KHR/khrplatform.h
│   └── src/glad.c
└── imgui-1.92.8/
    ├── imgui.cpp, imgui_draw.cpp, imgui_tables.cpp, imgui_widgets.cpp, imgui_demo.cpp
    ├── imgui.h, ...
    └── backends/
        ├── imgui_impl_glfw.cpp / .h
        └── imgui_impl_opengl3.cpp / .h   ← usamos OPENGL3 (Core Profile), no OPENGL2
```

**Importante:** en `lab10_boids` usabas `imgui_impl_opengl2` porque el pipeline
era immediate mode. Aquí pasamos a **Core Profile con shaders**, así que el
backend correcto es `imgui_impl_opengl3.cpp`. Si tu copia de `imgui-1.92.8`
solo tiene el backend `opengl2` úsalo igual, pero cambia en `main.cpp`:

```cpp
#include "backends/imgui_impl_opengl2.h"
...
ImGui_ImplOpenGL2_Init();          // sin argumento de version GLSL
...
ImGui_ImplOpenGL2_NewFrame();
...
ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
```

GLFW se linkea como librería del sistema (paquete de MSYS2 UCRT64,
`pacman -S mingw-w64-ucrt-x86_64-glfw`), igual que en tus otros labs — no
hace falta vendorearlo aquí.
