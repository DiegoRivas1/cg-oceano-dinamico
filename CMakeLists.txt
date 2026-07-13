cmake_minimum_required(VERSION 3.20)
project(cg_oceano_dinamico LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------
# Rutas compartidas (mismo patron que cg_labs)
# ------------------------------------------------------------------
set(EXTERNAL_DIR "${CMAKE_CURRENT_LIST_DIR}/external" CACHE PATH "Ruta a external/")
set(SHARED_DIR   "${CMAKE_CURRENT_LIST_DIR}/shared"   CACHE PATH "Ruta a shared/")

# Copia aqui (o symlink) tu carpeta imgui-1.92.8 y glad ya usados en cg_labs
set(IMGUI_DIR "${EXTERNAL_DIR}/imgui-1.92.8")
set(GLAD_DIR  "${EXTERNAL_DIR}/glad")

# ------------------------------------------------------------------
# Ejecutable principal
# ------------------------------------------------------------------
add_executable(cg_oceano
    src/main.cpp
    src/Ola.cpp
    src/GestorOlas.cpp
    src/Oceano.cpp
    src/Shader.cpp
    src/ObjetoRenderizable.cpp
    src/Camara.cpp
    src/Primitivas.cpp
    src/Barco.cpp
    src/Isla.cpp
    src/Faro.cpp
    src/Skybox.cpp
    src/ElementoEscenaFactory.cpp
    src/Escena.cpp

    ${GLAD_DIR}/src/glad.c

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(cg_oceano PRIVATE
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SHARED_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(cg_oceano PRIVATE
    glfw3
    opengl32
    gdi32
    glu32
)

# ------------------------------------------------------------------
# Copiar shaders/texturas/datos junto al ejecutable en cada build
# ------------------------------------------------------------------
add_custom_command(TARGET cg_oceano POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/shaders" "$<TARGET_FILE_DIR:cg_oceano>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/textures" "$<TARGET_FILE_DIR:cg_oceano>/textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/data" "$<TARGET_FILE_DIR:cg_oceano>/data"
)
