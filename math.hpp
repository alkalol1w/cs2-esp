#pragma once
#include <cmath>

struct Vector3 {
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    Vector3 operator-(const Vector3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vector3 operator+(const Vector3& o) const { return { x + o.x, y + o.y, z + o.z }; }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }
};

struct Vector2 {
    float x = 0, y = 0;
};

struct ViewMatrix {
    float m[4][4];
};

inline bool WorldToScreen(const Vector3& pos, Vector2& screen,
                          const ViewMatrix& vm, int width, int height)
{
    float w = vm.m[3][0] * pos.x + vm.m[3][1] * pos.y + vm.m[3][2] * pos.z + vm.m[3][3];
    if (w < 0.001f) return false;

    float x = vm.m[0][0] * pos.x + vm.m[0][1] * pos.y + vm.m[0][2] * pos.z + vm.m[0][3];
    float y = vm.m[1][0] * pos.x + vm.m[1][1] * pos.y + vm.m[1][2] * pos.z + vm.m[1][3];

    float invW = 1.0f / w;
    screen.x = (width  * 0.5f) + (x * invW) * (width  * 0.5f);
    screen.y = (height * 0.5f) - (y * invW) * (height * 0.5f);
    return true;
}