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

    Vector3F xyz() const
    {
        return Vector3F(x, y, z);
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

	Matrix4F transposed() const
	{
		return Matrix4F(
			Vector4F(c1.x, c2.x, c3.x, c4.x),
			Vector4F(c1.y, c2.y, c3.y, c4.y),
			Vector4F(c1.z, c2.z, c3.z, c4.z),
			Vector4F(c1.w, c2.w, c3.w, c4.w)
		);
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
        
    Vector4F operator*(const Vector4F &v) const
    {
        return c1*v.x + c2*v.y + c3*v.z + c4*v.w;
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
