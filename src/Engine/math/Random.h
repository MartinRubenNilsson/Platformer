#pragma once
#include "math/vec2.hpp"
#include <random>
#include <span>
#include <vector>
#include <initializer_list>


class Random
{
public:
	static bool Bernoulli(float p = 0.5f);

	static int    UniformInt(int a, int b);
	static float  UniformReal(float a, float b);
	static float2 UniformReal2(float2 a, float2 b);

	static float  Normal(float mean = 0.f, float sigma = 1.f);
	static float2 Normal2(float2 mean = { 0.f, 0.f }, float2 sigma = { 1.f, 1.f });

	template <class T> static T* Uniform(std::span<T>);
	template <class T> static T* Uniform(std::vector<T>&);
	template <class T> static T* Uniform(std::initializer_list<T>);

private:
	static std::default_random_engine ourEngine;
};


// Implementations

template<class T>
inline T* Random::Uniform(std::span<T> aSpan)
{
	return aSpan.empty() ? nullptr : &aSpan[UniformInt(0, static_cast<int>(aSpan.size() - 1))];
}

template<class T>
inline T* Random::Uniform(std::vector<T>& aVector)
{
	return Uniform<T>(std::span<T>(aVector));
}

template<class T>
inline T* Random::Uniform(std::initializer_list<T> aList)
{
	return Uniform<T>(std::span<T>(aList));
}
