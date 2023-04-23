#include "common.hlsli"
#include "vertices.hlsli"
#include "cbuffers.hlsli"
#include "textures.hlsli"


float4 main(VertexOut input) : SV_TARGET
{
    // https://gl-transitions.com/editor/Directional
    const float2 uv = input.uv + transitionProgress * sign(transitionDirection);
    const float4 oldColor = transitionTextureOld.Sample(defaultSampler, frac(uv));
    const float4 newColor = transitionTextureNew.Sample(defaultSampler, frac(uv));
    const float which = step(0.f, uv.x) * step(uv.x, 1.f) * step(0.f, uv.y) * step(uv.y, 1.f);
    return lerp(newColor, oldColor, which);
}