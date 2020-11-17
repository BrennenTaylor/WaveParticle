#define PI 3.14159265359
#define HALF_PI 1.57079632679
#define EPSILON 0.00024414;

#define N 7
#define N_HALF 3

#define TEXTURE_WIDTH 512

Texture2D Texture00 : register(t0);
Texture2D Texture01 : register(t1);
SamplerState CausticsSampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TexCoord;
};

struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0;
};

VS_OUTPUT VSMain(uint VertexID : SV_VertexID)
{
    VS_OUTPUT output;
    // Set texture coordinates
    // X: VertexID / 2 & 2
    // Y: VertexID & 2
    output.texCoord = float2( (VertexID << 1) & 2, VertexID & 2);
    output.position = float4(output.texCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

PS_OUTPUT PSMain(VS_OUTPUT input)
{
    PS_OUTPUT output;
    float val = 0.0f;
    val += Texture00.SampleLevel(CausticsSampler, input.texCoord + float2(0.0f, -3.0f / TEXTURE_WIDTH), 0).r;
    val += Texture00.SampleLevel(CausticsSampler, input.texCoord + float2(0.0f, -2.0f / TEXTURE_WIDTH), 0).g;
    val += Texture00.SampleLevel(CausticsSampler, input.texCoord + float2(0.0f, -1.0f / TEXTURE_WIDTH), 0).b;
    val += Texture00.SampleLevel(CausticsSampler, input.texCoord, 0).a;
    val += Texture01.SampleLevel(CausticsSampler, input.texCoord + float2(0.0f, 1.0f / TEXTURE_WIDTH), 0).r;
    val += Texture01.SampleLevel(CausticsSampler, input.texCoord + float2(0.0f, 2.0f / TEXTURE_WIDTH), 0).g;
    val += Texture01.SampleLevel(CausticsSampler, input.texCoord + float2(0.0f, 3.0f / TEXTURE_WIDTH), 0).b;
    output.color0 = float4(val, val, val, val);
    return output;
}
