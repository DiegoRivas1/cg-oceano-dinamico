// main.cpp
// Sistema de Animacion por Computador de un Oceano
// GLFW (ventana/input) + GLAD (carga de OpenGL Core Profile) + ImGui (panel de control)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <algorithm>
#include <iostream>

#include "Escena.h"

namespace {
    Escena* g_escena = nullptr;
    bool g_arrastrando = false;
    double g_ultimoX = 0.0, g_ultimoY = 0.0;
    float g_velocidadAnimacion = 1.0f;
}

void callbackScroll(GLFWwindow*, double, double yOffset) {
    if (g_escena) g_escena->camara().acercar(static_cast<float>(-yOffset) * 1.5f);
}

void callbackMouseBoton(GLFWwindow* ventana, int boton, int accion, int) {
    if (boton == GLFW_MOUSE_BUTTON_LEFT) {
        g_arrastrando = (accion == GLFW_PRESS);
        glfwGetCursorPos(ventana, &g_ultimoX, &g_ultimoY);
    }
}

void callbackMouseMover(GLFWwindow*, double x, double y) {
    if (g_arrastrando && g_escena) {
        float deltaX = static_cast<float>(x - g_ultimoX) * 0.005f;
        float deltaY = static_cast<float>(y - g_ultimoY) * 0.005f;
        g_escena->camara().rotar(deltaX, deltaY);
    }
    g_ultimoX = x;
    g_ultimoY = y;
}

void procesarTeclado(GLFWwindow* ventana) {
    static bool teclaTPresionada = false, teclaLPresionada = false;

    if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(ventana, true);

    // Activar/desactivar textura (tecla T)
    if (glfwGetKey(ventana, GLFW_KEY_T) == GLFW_PRESS) {
        if (!teclaTPresionada) { g_escena->oceano().alternarTextura(); teclaTPresionada = true; }
    } else teclaTPresionada = false;

    // Activar/desactivar iluminacion (tecla L)
    if (glfwGetKey(ventana, GLFW_KEY_L) == GLFW_PRESS) {
        if (!teclaLPresionada) { g_escena->oceano().alternarIluminacion(); teclaLPresionada = true; }
    } else teclaLPresionada = false;

    // Aumentar/disminuir velocidad de animacion (teclado numerico + / -)
    if (glfwGetKey(ventana, GLFW_KEY_KP_ADD) == GLFW_PRESS) g_velocidadAnimacion += 0.01f;
    if (glfwGetKey(ventana, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) g_velocidadAnimacion = std::max(0.0f, g_velocidadAnimacion - 0.01f);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "No se pudo inicializar GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* ventana = glfwCreateWindow(1280, 720, "CG - Oceano Dinamico", nullptr, nullptr);
    if (!ventana) {
        std::cerr << "No se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(ventana);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "No se pudo inicializar GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glfwSetScrollCallback(ventana, callbackScroll);
    glfwSetMouseButtonCallback(ventana, callbackMouseBoton);
    glfwSetCursorPosCallback(ventana, callbackMouseMover);

    // --- ImGui ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(ventana, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    Escena escena;
    g_escena = &escena;

    double tiempoAnterior = glfwGetTime();

    while (!glfwWindowShouldClose(ventana)) {
        double tiempoActual = glfwGetTime();
        float deltaTiempo = static_cast<float>(tiempoActual - tiempoAnterior);
        tiempoAnterior = tiempoActual;

        glfwPollEvents();
        procesarTeclado(ventana);
        escena.oceano().setVelocidad(g_velocidadAnimacion);
        escena.actualizar(deltaTiempo);

        int ancho, alto;
        glfwGetFramebufferSize(ventana, &ancho, &alto);
        glViewport(0, 0, ancho, alto);
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspecto = alto > 0 ? static_cast<float>(ancho) / alto : 1.0f;
        escena.dibujar(aspecto);

        // --- Panel ImGui ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Control del Oceano");
        ImGui::SliderFloat("Velocidad animacion", &g_velocidadAnimacion, 0.0f, 3.0f);
        ImGui::Text("Controles: T = textura, L = iluminacion, +/- = velocidad");
        ImGui::Text("Mouse: arrastrar = rotar camara, scroll = zoom");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(ventana);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(ventana);
    glfwTerminate();
    return 0;
}
