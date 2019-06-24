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

    friend inline Vector4F operator*(float s, const Vector4F &v)
    {
        return Vector4F(s*v.x, s*v.y, s*v.z, s*v.w);
    }

    friend inline Vector4F operator*(const Vector4F &v, float s)
    {
        return s*v;
    }

    Vector4F operator+(const Vector4F &o) const
    {
        return Vector4F(x+o.x, y+o.y, z+o.z, w+o.w);
    }

    Vector4F operator-(const Vector4F &o) const
    {
        return Vector4F(x-o.x, y-o.y, z-o.z, w-o.w);
    }

    Vector4F operator*(const Vector4F &o) const
    {
        return Vector4F(x*o.x, y*o.y, z*o.z, w*o.w);
    }

    float x, y, z, w;
};

struct Matrix4F
{
    Matrix4F() {}
    Matrix4F(const Vector4F &v1, const Vector4F &v2, const Vector4F &v3, const Vector4F &v4)
        : c1(v1), c2(v2), c3(v3), c4(v4) {}

    static inline Matrix4F identity()
    {
        return Matrix4F(
            Vector4F(1.0f, 0.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 1.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 1.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    Matrix4F operator*(const Matrix4F &o) const
    {
        return Matrix4F(
            c1.x*o.c1 + c2.x*o.c1 + c3.x*o.c1 + c4.x*o.c1,
            c1.y*o.c2 + c2.y*o.c2 + c3.y*o.c2 + c4.y*o.c2,
            c1.z*o.c3 + c2.z*o.c3 + c3.z*o.c3 + c4.z*o.c3,
            c1.w*o.c4 + c2.w*o.c4 + c3.w*o.c4 + c4.w*o.c4
        );
    }

    Matrix4F &operator *=(const Matrix4F &o)
    {
        (*this) = (*this) * o;
        return *this;
    }

    Vector4F c1, c2, c3, c4;
};
} // End of namespace AgpuCommon

#endif //AGPU_VECTOR_MATH_HPP
