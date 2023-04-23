#include "common.hlsli"
#include "vertices.hlsli"
#include "cbuffers.hlsli"


VertexOut main(VertexIn input)
{
    float2 vertex = lineStripVertices[input.vertexID].xy;
    
    VertexOut output;
    
    // Transform vertex coords to camera space
    float3x3 worldToCamera = GetInvTransform(cameraSize / 2.f, 0.f, cameraPosition);
    vertex = mul(float3(vertex, 1.f), worldToCamera).xy;
    
    output.position = float4(vertex.x, -vertex.y, 0.f, 1.f); // Flip y-axis
    output.uv = float2(1.f, 1.f);

    return output;
}