#include "pch.h"
#include "math/PerlinNoise.h"
#include <cmath>

/*
* https://en.wikipedia.org/wiki/Perlin_noise#Implementation
*/


// Public static methods

float PerlinNoise::Get(float2 p)
{
    const int2 i = (int2)floor(p);

    const float d00 = GetDotGradient(i + int2(0, 0), p);
    const float d10 = GetDotGradient(i + int2(1, 0), p);
    const float d01 = GetDotGradient(i + int2(0, 1), p);
    const float d11 = GetDotGradient(i + int2(1, 1), p);

    const float2 s = frac(p);

    const float d0 = Interpolate(d00, d10, s.x);
    const float d1 = Interpolate(d01, d11, s.x);

    return Interpolate(d0, d1, s.y);
}


// Private static methods

float2 PerlinNoise::GetGradient(int2 i)
{
    constexpr unsigned w = 8 * sizeof(unsigned);
    constexpr unsigned s = w / 2; // rotation width

    unsigned a = i.x;
    unsigned b = i.y;

    a *= 3284157443;
    b ^= a << s | a >> w - s;
    b *= 1911520717;
    a ^= b << s | b >> w - s;
    a *= 2048419325;

    const float random = a * (3.14159265f / ~(~0u >> 1)); // in [0, 2*Pi]

    return { std::cos(random), std::sin(random) };
}

float PerlinNoise::GetDotGradient(int2 i, float2 p)
{
    return dot(GetGradient(i), p - (float2)i);
}

float PerlinNoise::Interpolate(float x, float y, float s)
{
    return x + (y - x) * ((s * (s * 6.f - 15.f) + 10.f) * s * s * s);
}
