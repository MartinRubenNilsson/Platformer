#include "common.hlsli"
#include "vertices.hlsli"
#include "cbuffers.hlsli"


VertexOut main(VertexIn input)
{
    // OBS: World space is x-right y-down while clip space is x-right y-up
    
    const float2 vertices[] =
    {
        { 0.f, 0.f },
        { 1.f, 0.f },
        { 0.f, 1.f },
        
        { 1.f, 1.f },
        { 0.f, 1.f },
        { 1.f, 0.f }
    };
    
    float2 vertex = vertices[input.vertexID];
    
    VertexOut output;
    
    // Compute texture UV
    output.uv = spriteMinUV + vertex * (spriteMaxUV - spriteMinUV); // Lerp
    
    // Take into account pivot
    vertex -= spritePivot;
    
    // Transform vertex coords from sprite space to camera space
    float3x3 spriteToWorld = GetTransform(spriteSize, spriteRotation, spritePosition);
    float3x3 worldToCamera = GetInvTransform(cameraSize / 2.f, 0.f, cameraPosition);
    vertex = mul(mul(float3(vertex, 1.f), spriteToWorld), worldToCamera).xy;
    
    output.position = float4(vertex.x, -vertex.y, 0.f, 1.f); // Flip y-axis

    return output;
}