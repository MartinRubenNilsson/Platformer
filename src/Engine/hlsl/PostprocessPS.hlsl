#include "common.hlsli"
#include "vertices.hlsli"
#include "cbuffers.hlsli"
#include "textures.hlsli"


float4 main(VertexOut input) : SV_TARGET
{
    float4 color = postprocessTexture.Sample(defaultSampler, input.uv);
	return color * postprocessTint;
}