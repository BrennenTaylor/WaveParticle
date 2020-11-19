cbuffer cbPerObject
{
    float4x4 WVP;
};

cbuffer cbPerCamera
{
    float3 eyeWS;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D CausticTexture : register(t0);
SamplerState ClampSamplerState;

static float3 surfaceColor = float3(194 / 255.0f, 178 / 255.0f, 128 / 255.0f);

// For now, we simply project full texture map to entire grid
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    // No change here
    float3 newPos = input.position;
    output.position = mul(float4(newPos, 1.0f), WVP);
    output.color = input.color;
    output.uv = input.uv;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float3 textureColor = CausticTexture.SampleLevel(ClampSamplerState, input.uv, 0).xyz * surfaceColor;
    return float4(textureColor, 1.0);
}
