struct VertexIn
{
    uint vertexID : SV_VertexID;
};

struct VertexOut
{
    float4 position : SV_Position; // Is converted from clip space to target space by rasterizer.
    float2 uv : TEXCOORD;
};