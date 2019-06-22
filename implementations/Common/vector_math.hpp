#ifndef AGPU_VECTOR_MATH_HPP
#define AGPU_VECTOR_MATH_HPP

namespace AgpuCommon
{

struct Vector2F
{
    Vector2F(float cx = 0.0f, float cy = 0.0f)
        : x(cx), y(cy) {}
    float x, y;
};

struct Vector3F
{
    Vector3F(float cx = 0.0f, float cy = 0.0f, float cz = 0.0f)
        : x(cx), y(cy), z(cz) {}
    float x, y, z;
};

struct Vector4F
{
    Vector4F(float cx = 0.0f, float cy = 0.0f, float cz = 0.0f, float cw=0.0f)
        : x(cx), y(cy), z(cz), w(cw) {}

    float x, y, z, w;
};

} // End of namespace AgpuCommon

#endif //AGPU_VECTOR_MATH_HPP
