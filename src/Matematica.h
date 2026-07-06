#pragma once
// Matematica.h
// Vector3 y Matriz4x4 minimos para no depender de glm.
// Usamos Core Profile + shaders, por lo que ya no podemos apoyarnos
// en la pila de matrices fija de OpenGL (glMultMatrix, gluPerspective, etc).
// Layout column-major, compatible directo con glUniformMatrix4fv(transpose=GL_FALSE).

#include <array>
#include <cmath>

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }

    static Vec3 cruz(const Vec3& a, const Vec3& b) {
        return { a.y * b.z - a.z * b.y,
                 a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x };
    }

    static float punto(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Vec3 normalizado() const {
        float len = std::sqrt(x * x + y * y + z * z);
        if (len < 1e-6f) return {0.0f, 0.0f, 0.0f};
        return { x / len, y / len, z / len };
    }
};

struct Mat4 {
    std::array<float, 16> m{};

    static Mat4 identidad() {
        Mat4 r;
        r.m = {1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1};
        return r;
    }

    static Mat4 trasladar(const Vec3& t) {
        Mat4 r = identidad();
        r.m[12] = t.x; r.m[13] = t.y; r.m[14] = t.z;
        return r;
    }

    static Mat4 escalar(const Vec3& s) {
        Mat4 r = identidad();
        r.m[0] = s.x; r.m[5] = s.y; r.m[10] = s.z;
        return r;
    }

    static Mat4 rotarY(float radianes) {
        Mat4 r = identidad();
        float c = std::cos(radianes), s = std::sin(radianes);
        r.m[0] = c;  r.m[2] = s;
        r.m[8] = -s; r.m[10] = c;
        return r;
    }

    // fovYRad: campo de vision vertical en radianes
    static Mat4 perspectiva(float fovYRad, float aspecto, float cercano, float lejano) {
        Mat4 r{};
        float f = 1.0f / std::tan(fovYRad / 2.0f);
        r.m[0]  = f / aspecto;
        r.m[5]  = f;
        r.m[10] = (lejano + cercano) / (cercano - lejano);
        r.m[11] = -1.0f;
        r.m[14] = (2.0f * lejano * cercano) / (cercano - lejano);
        return r;
    }

    static Mat4 mirarHacia(const Vec3& ojo, const Vec3& objetivo, const Vec3& arriba) {
        Vec3 f = (objetivo - ojo).normalizado();
        Vec3 s = Vec3::cruz(f, arriba).normalizado();
        Vec3 u = Vec3::cruz(s, f);

        Mat4 r = identidad();
        r.m[0] = s.x;  r.m[4] = s.y;  r.m[8]  = s.z;
        r.m[1] = u.x;  r.m[5] = u.y;  r.m[9]  = u.z;
        r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
        r.m[12] = -Vec3::punto(s, ojo);
        r.m[13] = -Vec3::punto(u, ojo);
        r.m[14] =  Vec3::punto(f, ojo);
        return r;
    }

    Mat4 operator*(const Mat4& o) const {
        Mat4 r{};
        for (int col = 0; col < 4; ++col) {
            for (int fila = 0; fila < 4; ++fila) {
                float suma = 0.0f;
                for (int k = 0; k < 4; ++k) suma += m[k * 4 + fila] * o.m[col * 4 + k];
                r.m[col * 4 + fila] = suma;
            }
        }
        return r;
    }
};
