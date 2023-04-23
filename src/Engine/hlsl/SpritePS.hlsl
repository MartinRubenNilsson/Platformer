#include "common.hlsli"
#include "vertices.hlsli"
#include "cbuffers.hlsli"
#include "textures.hlsli"


float4 main(VertexOut input) : SV_TARGET
{
    float4 color = spriteTexture.Sample(defaultSampler, input.uv);
    color = lerp(color, 1, spriteTint); // Apply tint
    color.a *= spriteAlpha; // Apply alpha
    
    return color;
}