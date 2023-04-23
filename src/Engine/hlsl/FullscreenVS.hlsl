#include "vertices.hlsli"


VertexOut main(VertexIn input)
{
    const float2 vertices[] = { { -1.f, 1.f }, { 3.f, 1.f }, { -1.f, -3.f } };
    const float2 uvs[] = { { 0.f, 0.f }, { 2.f, 0.f }, { 0.f, 2.f } };
    
    VertexOut output;
    output.position = float4(vertices[input.vertexID], 0.f, 1.f);
    output.uv = uvs[input.vertexID];
	
	return output;
}