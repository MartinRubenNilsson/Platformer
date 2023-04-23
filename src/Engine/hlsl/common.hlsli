sampler defaultSampler : register(s0);


// Functions

//  Order is: Scale-rotate-translate (assuming row vectors)
float3x3 GetTransform(float2 aScale, float aRotationInRadians, float2 aTranslation)
{
    float s, c;
    sincos(aRotationInRadians, s, c);
    return float3x3(
        aScale.x * float2(c, s), 0.f,
        aScale.y * float2(-s, c), 0.f,
        aTranslation, 1.f
    );
}

// Also assuming row vectors
float3x3 GetInvTransform(float2 aScale, float aRotationInRadians, float2 aTranslation)
{
    float s, c;
    sincos(aRotationInRadians, s, c);
    float2 top = { c / aScale.x, -s / aScale.y };
    float2 mid = { s / aScale.x, c / aScale.y };
    return float3x3(
        top, 0.f,
        mid, 0.f,
        -aTranslation.x * top - aTranslation.y * mid, 1.f
    );
}