#pragma once
#include "math/vec2.hpp"


template <class T>
struct mat2x2
{
	float a[2][2] = {};

		  T& operator()(size_t i, size_t j)		  { return a[i][j]; }
	const T& operator()(size_t i, size_t j) const { return a[i][j]; }
};


// Typedefs

using int2x2 = mat2x2<int>;
using uint2x2 = mat2x2<unsigned>;
using float2x2 = mat2x2<float>;


// Unary operators

template <class T>
mat2x2<T> operator-(const mat2x2<T>& A)
{
	return { -A(0, 0), -A(0, 1), -A(1, 0), -A(1, 1) };
}


// Binary operators

template <class T>
mat2x2<T> operator+(const mat2x2<T>& A, const mat2x2<T>& B)
{
	return { A(0, 0) + B(0, 0), A(0, 1) + B(0, 1), A(1, 0) + B(1, 0), A(1, 1) + B(1, 1) };
}

template <class T>
mat2x2<T> operator-(const mat2x2<T>& A, const mat2x2<T>& B)
{
	return { A(0, 0) - B(0, 0), A(0, 1) - B(0, 1), A(1, 0) - B(1, 0), A(1, 1) - B(1, 1) };
}

template <class T>
mat2x2<T> operator*(const mat2x2<T>& A, const mat2x2<T>& B)
{
	return
	{
		A(0, 0) * B(0, 0) + A(0, 1) * B(1, 0),
		A(1, 0) * B(0, 0) + A(1, 1) * B(1, 0),
		A(0, 0) * B(0, 1) + A(0, 1) * B(1, 1),
		A(1, 0) * B(0, 1) + A(1, 1) * B(1, 1),
	};
}
