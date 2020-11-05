cbuffer cbPerObject
{
    float4x4 WVP;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 color : COLOR;
    float4 position : SV_POSITION;
    float3 normal : NORMALS;
    float amplitude : AMPLITUDE;
};

Texture2D ParticleTexture;
SamplerState ParticleTextureSamplerState;

// For now, we simply project full texture map to entire grid
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    // No change here
    float3 newPos = input.position;
    float3 readSample = ParticleTexture.SampleLevel(ParticleTextureSamplerState, input.uv, 0).xyz;
    newPos.y = readSample.y;

    float small = 1.0f / 1000.0f;

    float2 leftSampleUV = float2(input.uv.x - small, input.uv.y);
    float2 rightSampleUV = float2(input.uv.x + small, input.uv.y);

    float2 topSampleUV = float2(input.uv.x, input.uv.y - small);
    float2 bottomSampleUV = float2(input.uv.x, input.uv.y + small);

    float3 leftSample = ParticleTexture.SampleLevel(ParticleTextureSamplerState, leftSampleUV, 0).xyz;
    float3 rightSample = ParticleTexture.SampleLevel(ParticleTextureSamplerState, rightSampleUV, 0).xyz;

    float3 topSample = ParticleTexture.SampleLevel(ParticleTextureSamplerState, topSampleUV, 0).xyz;
    float3 bottomSample = ParticleTexture.SampleLevel(ParticleTextureSamplerState, bottomSampleUV, 0).xyz;


    // NOTE: The 100 here is the grid size in the x and y direction. This allows us to calculate the positions required for the normal
    float3 left = float3(leftSampleUV.x * 100, leftSample.y, leftSampleUV.y * 100);
    float3 right = float3(rightSampleUV.x * 100, rightSample.y, rightSampleUV.y * 100);
    float3 top = float3(topSampleUV.x * 100,  topSample.y, topSampleUV.y * 100);
    float3 bottom = float3(bottomSampleUV.x * 100, bottomSample.y, bottomSampleUV.y * 100);

    float3 xNorm =  right - left;
    float3 zNorm =  bottom - top;

    // NOTE: We dont need to project this because we dont do a world matrix transform on the grid
    output.normal = normalize(cross(zNorm, xNorm));

    output.amplitude = newPos.y;

    output.position = mul(float4(newPos, 1.0f), WVP);
    output.color = input.color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float3 norm = normalize(input.normal);

    // Directional Light
    float3 lightDir = normalize(float3(1.0f, -1.0f, 1.0f));
    float3 L = -lightDir;

    float nDotL = saturate(dot(input.normal, L));
    float3 diffuse = nDotL * float3(0.0f, 1.0f, 1.0f);

    return float4(diffuse, 1.0f);
}
