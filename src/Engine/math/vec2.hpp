#pragma once
#include <cmath>
#include <type_traits>


template <class T>
struct vec2
{
	T x = {};
	T y = {};

	template <class T1>
	explicit operator vec2<T1>() const { return { static_cast<T1>(x), static_cast<T1>(y) }; }
};


// Typedefs

using int2 = vec2<int>;
using uint2 = vec2<unsigned>;
using float2 = vec2<float>;


// Unary operators

template <class T> vec2<T> operator-(const vec2<T>& v) { return { -v.x, -v.y }; }


// Binary operators

#define TEMPLATE template <class T1, class T2> vec2<std::common_type_t<T1, T2>>
#define VECTOR_VECTOR(OP) TEMPLATE operator##OP##(const vec2<T1> &l, const vec2<T2> &r) { return { l.x OP r.x, l.y OP r.y }; }
#define VECTOR_SCALAR(OP) TEMPLATE operator##OP##(const vec2<T1> &l, const      T2  &r) { return { l.x OP r,   l.y OP r   }; }
#define SCALAR_VECTOR(OP) TEMPLATE operator##OP##(const      T1  &l, const vec2<T2> &r) { return { l   OP r.x, l   OP r.y }; }
#define ASSIGNMENT_VECTOR(OP) template<class T1, class T2> vec2<T1>& operator##OP##=(vec2<T1> &l, const vec2<T2> &r) { l = l OP r;              return l; }
#define ASSIGNMENT_SCALAR(OP) template<class T1, class T2> vec2<T1>& operator##OP##=(vec2<T1> &l, const      T2  &r) { l = l OP vec2<T2>(r, r); return l; }
#define DEFINE_ALL(OP) VECTOR_VECTOR(OP) VECTOR_SCALAR(OP) SCALAR_VECTOR(OP) ASSIGNMENT_VECTOR(OP) ASSIGNMENT_SCALAR(OP)

DEFINE_ALL(+)
DEFINE_ALL(-)
DEFINE_ALL(*)
DEFINE_ALL(/)
DEFINE_ALL(%)

#undef TEMPLATE
#undef VECTOR_VECTOR
#undef VECTOR_SCALAR
#undef SCALAR_VECTOR
#undef ASSIGNMENT_VECTOR
#undef ASSIGNMENT_SCALAR
#undef DEFINE_ALL


// Comparison operators

template <class T>
bool operator==(const vec2<T>& lhs, const vec2<T>& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <class T>
bool operator!=(const vec2<T>& lhs, const vec2<T>& rhs)
{
	return !(lhs == rhs);
}


// Functions

template <class T>
vec2<T> min(const vec2<T>& a, const vec2<T>& b)
{
	return { std::min(a.x, b.x), std::min(a.y, b.y) };
}

template <class T>
vec2<T> max(const vec2<T>& a, const vec2<T>& b)
{
	return { std::max(a.x, b.x), std::max(a.y, b.y) };
}

inline float2 floor(float2 v)
{
	return { std::floor(v.x), std::floor(v.y) };
}

inline float2 frac(float2 v)
{
	return v - floor(v);
}

inline float2 abs(float2 v)
{
	return { std::abs(v.x), std::abs(v.y) };
}

inline float length(float2 v)
{
	return std::sqrtf(v.x * v.x + v.y * v.y);
}

inline float2 normalize(float2 v)
{
	return v / length(v);
}

inline float2 lerp(float2 x, float2 y, float t)
{
	return x * (1.f - t) + y * t;
}

inline float2 rotate(float2 v, float r) // r is in radians
{
	return { cos(r) * v.x - sin(r) * v.y, sin(r) * v.x + cos(r) * v.y };
}

inline float dot(float2 u, float2 v)
{
	return u.x * v.x + u.y * v.y;
}

inline float determinant(float2 u, float2 v)
{
	return u.x * v.y - u.y * v.x;
}

inline float2 reflect(float2 i, float2 n)
{
	return i - 2.f * dot(i, n) * n;
}