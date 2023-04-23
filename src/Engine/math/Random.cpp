#include "pch.h"
#include "math/Random.h"


// Static members

std::default_random_engine Random::ourEngine{ std::random_device{}() };


// Public static methods

bool Random::Bernoulli(float p)
{
    return std::bernoulli_distribution{ static_cast<double>(p) }(ourEngine);
}

int Random::UniformInt(int a, int b)
{
    return std::uniform_int_distribution<int>{a, b}(ourEngine);
}

float Random::UniformReal(float a, float b)
{
    return std::uniform_real_distribution<float>{a, b}(ourEngine);
}

float2 Random::UniformReal2(float2 a, float2 b)
{
    return { UniformReal(a.x, b.x), UniformReal(a.y, b.y) };
}

float Random::Normal(float mean, float sigma)
{
    return std::normal_distribution<float>{mean, sigma}(ourEngine);
}

float2 Random::Normal2(float2 mean, float2 sigma)
{
    return { Normal(mean.x, sigma.x), Normal(mean.y, sigma.y) };
}
