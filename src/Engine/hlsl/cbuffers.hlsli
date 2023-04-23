cbuffer CameraData : register(b0)
{
    float2 cameraSize;
    float2 cameraPosition;
};

cbuffer SpriteBuffer : register(b1)
{
    float2 spriteMinUV;
    float2 spriteMaxUV;
    float2 spritePivot;
    float2 spriteSize;
    float2 spritePosition;
    float spriteRotation;
    float spriteAlpha;
    float4 spriteTint;
};

cbuffer LineStripBuffer : register(b2)
{
    float4 lineStripVertices[8];
};

cbuffer TransitionBuffer : register(b3)
{
    float transitionProgress;
    float garbage0;
    float2 transitionDirection;
}

cbuffer PostprocessBuffer : register(b4)
{
    float4 postprocessTint;
}