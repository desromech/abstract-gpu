#ifndef AGPU_VECTOR_MATH_HPP
#define AGPU_VECTOR_MATH_HPP

#include <functional>
#include "AGPU/agpu.h"

namespace AgpuCommon
{

struct Vector2F
{
    Vector2F(float cx = 0.0f, float cy = 0.0f)
        : x(cx), y(cy) {}

    size_t hash() const
    {
        return std::hash<float> ()(x) ^ std::hash<float> ()(y);
    }

    bool operator==(const Vector2F &o) const
    {
        return x == o.x && y == o.y;
    }

    float x, y;
};

struct Vector3F
{
    explicit Vector3F(const agpu_vector3f &v)
        : x(v.x), y(v.y), z(v.z) {}

    Vector3F(float cx = 0.0f, float cy = 0.0f, float cz = 0.0f)
        : x(cx), y(cy), z(cz) {}

    size_t hash() const
    {
        return std::hash<float> ()(x) ^ std::hash<float> ()(y) ^ std::hash<float> ()(z);
    }

    bool operator==(const Vector3F &o) const
    {
        return x == o.x && y == o.y && z == o.z;
    }

    friend inline Vector3F operator*(float s, const Vector3F &v)
    {
        return Vector3F(s*v.x, s*v.y, s*v.z);
    }

    friend inline Vector3F operator*(const Vector3F &v, float s)
    {
        return s*v;
    }

    Vector3F operator+(const Vector3F &o) const
    {
        return Vector3F(x+o.x, y+o.y, z+o.z);
    }

    Vector3F operator-(const Vector3F &o) const
    {
        return Vector3F(x-o.x, y-o.y, z-o.z);
    }

    Vector3F operator*(const Vector3F &o) const
    {
        return Vector3F(x*o.x, y*o.y, z*o.z);
    }

    float x, y, z;
};

struct Vector4F
{
    explicit Vector4F(const agpu_vector4f &v)
        : x(v.x), y(v.y), z(v.z), w(v.w){}

    explicit Vector4F(const agpu_vector3f &v, float cw)
        : x(v.x), y(v.y), z(v.z), w(cw){}

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

    size_t hash() const
    {
        return std::hash<float> ()(x) ^ std::hash<float> ()(y)
            ^ std::hash<float> ()(z) ^ std::hash<float> ()(w);
    }

    bool operator==(const Vector4F &o) const
    {
        return x == o.x && y == o.y && z == o.z && w == o.w;
    }

    bool operator!=(const Vector4F &o) const
    {
        return x != o.x || y != o.y || z != o.z || w != o.w;
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

    float dot(const Vector4F &o) const
    {
        return x*o.x + y*o.y + z*o.z + w*o.w;
    }

    Vector3F xyz() const
    {
        return Vector3F(x, y, z);
    }

    union
    {
        struct
        {
            float x, y, z, w;
        };

        float values[4];
    };
};

struct Matrix4F
{
    Matrix4F() {}
    Matrix4F(const Matrix4F &o)
        : c1(o.c1), c2(o.c2), c3(o.c3), c4(o.c4) {}
    Matrix4F(const Vector4F &v1, const Vector4F &v2, const Vector4F &v3, const Vector4F &v4)
        : c1(v1), c2(v2), c3(v3), c4(v4) {}

    static inline Matrix4F zeros()
    {
        return Matrix4F(
            Vector4F(0.0f, 0.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 0.0f, 0.0f)
        );
    }

    static inline Matrix4F identity()
    {
        return Matrix4F(
            Vector4F(1.0f, 0.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 1.0f, 0.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 1.0f, 0.0f),
            Vector4F(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

	Matrix4F transposed() const
	{
		return Matrix4F(
			Vector4F(c1.x, c2.x, c3.x, c4.x),
			Vector4F(c1.y, c2.y, c3.y, c4.y),
			Vector4F(c1.z, c2.z, c3.z, c4.z),
			Vector4F(c1.w, c2.w, c3.w, c4.w)
		);
	}

    void luDecomposeInto(Matrix4F &L, Matrix4F &U) const
    {
        auto lc1 = c1;
        auto lc2 = c2;
        auto lc3 = c3;
        auto lc4 = c4;

        U = identity();

        // Second column
        U.c2.x = lc2.x / lc1.x;
        U.c3.x = lc3.x / lc1.x;
        U.c4.x = lc4.x / lc1.x;

        lc2 = lc2 - (lc1 * U.c2.x);
        lc3 = lc3 - (lc1 * U.c3.x);
        lc4 = lc4 - (lc1 * U.c4.x);

        // Third column
        U.c3.y = lc3.y / lc2.y;
        U.c4.y = lc4.y / lc2.y;

        lc3 = lc3 - (lc2 * U.c3.y);
        lc4 = lc4 - (lc2 * U.c4.y);

        // Fourth column
        U.c4.z = lc4.z / lc3.z;
        lc4 = lc4 - (lc3 * U.c4.z);

        L = Matrix4F(lc1, lc2, lc3, lc4);
    }

    Matrix4F lowerTriangularInverted() const
    {
        auto i = zeros();
        i.c1.x = 1.0f / c1.x;

        i.c1.y = - (c1.y * i.c1.x) / c2.y;
        i.c2.y = 1.0f / c2.y;

        i.c1.z = -((c1.z * i.c1.x) + (c2.z * i.c1.y)) / c3.z;
        i.c2.z = -(c2.z * i.c2.y) / c3.z;
        i.c3.z = 1.0f / c3.z;

        i.c1.w = -((c1.w * i.c1.x) + (c2.w * i.c1.y) + (c3.w * i.c3.z)) / c4.w;
        i.c2.w = -((c1.w * i.c2.y) + (c3.w * i.c2.z)) / c4.w;
        i.c3.w = -(c2.w * i.c3.z) / c4.w;
        i.c4.w = 1.0f / c4.w;

        /*
        <inline>
let i := WMMatrix4 newValue.

i
    m11: 1.0 / m11;

    m21: (m21 * i m11) negated / m22;
    m22: 1.0 / m22;

    m31: ((m31 * i m11) + (m32 * i m21)) negated / m33;
    m32: (m32 * i m22) negated / m33;
    m33: 1.0 / m33;

    m41: ((m41 * i m11) + (m42 * i m21) + (m43 * i m31)) negated / m44;
    m42: ((m42 * i m22) + (m43 * i m32)) negated / m44;
    m43: ((m43 * i m33)) negated / m44;
    m44: 1.0 / m44.

^ i*/
        return i;
    }

    Matrix4F upperTriangularInverted() const
    {
        auto i = zeros();

        i.c4.w = 1.0f / c4.w;

        i.c3.z = 1.0f / c3.z;
        i.c4.z = -(c4.z * i.c4.w) / c3.z;

        i.c2.y = 1.0f / c2.y;
        i.c3.y = -(c3.y * i.c3.z) / c2.y;
        i.c4.y = -((c3.y * i.c4.z) + (c4.y * i.c4.w)) / c2.y;

        i.c1.x = 1.0f / c1.x;
        i.c2.x = -(c2.x * i.c2.y) / c1.x;
        i.c3.x = -((c2.x * i.c3.y) + (c3.x * i.c3.z)) / c1.x;
        i.c4.x = -((c2.x * i.c4.y) + (c3.x * i.c4.z) + (c4.x * i.c4.w)) / c1.x;

        return i;
    }

    Matrix4F inverted() const
    {
        Matrix4F L;
        Matrix4F U;
        luDecomposeInto(L, U);

        return U.upperTriangularInverted() * L.lowerTriangularInverted();
    }

    size_t hash() const
    {
        return c1.hash() ^ c2.hash() ^ c3.hash() ^ c4.hash();
    }

    bool operator==(const Matrix4F &o) const
    {
        return c1 == o.c1 && c2 == o.c2 && c3 == o.c3 && c4 == o.c4;
    }

    Matrix4F operator+(const Matrix4F &o) const
    {
        return Matrix4F(c1+o.c1, c2+o.c2, c3+o.c3, c4+o.c4);
    }

    Matrix4F operator-(const Matrix4F &o) const
    {
        return Matrix4F(c1-o.c1, c2-o.c2, c3-o.c3, c4-o.c4);
    }

    Matrix4F operator*(const Matrix4F &o) const
    {
        return Matrix4F(
			c1*o.c1.x + c2*o.c1.y + c3*o.c1.z + c4*o.c1.w,
			c1*o.c2.x + c2*o.c2.y + c3*o.c2.z + c4*o.c2.w,
			c1*o.c3.x + c2*o.c3.y + c3*o.c3.z + c4*o.c3.w,
			c1*o.c4.x + c2*o.c4.y + c3*o.c4.z + c4*o.c4.w
        );
    }

    Vector3F transformDirection3(const Vector3F &v) const
    {
        return c1.xyz()*v.x + c2.xyz()*v.y + c3.xyz()*v.z;
    }

    friend Vector4F operator*(const Matrix4F &m, const Vector4F &v)
    {
        return m.c1*v.x + m.c2*v.y + m.c3*v.z + m.c4*v.w;
    }

    friend Vector4F operator*(const Vector4F &v, const Matrix4F &m)
    {
        return Vector4F(v.dot(m.c1), v.dot(m.c2), v.dot(m.c3), v.dot(m.c4));
    }

    Matrix4F &operator *=(const Matrix4F &o)
    {
        (*this) = (*this) * o;
        return *this;
    }

    float at(int x, int y) const
    {
        return columnAt(y).values[x];
    }

    const Vector4F &columnAt(int index) const
    {
        return (&c1)[index];
    }

    Vector4F c1, c2, c3, c4;

};
} // End of namespace AgpuCommon

#endif //AGPU_VECTOR_MATH_HPP
