#define PI 3.14159265359
#define HALF_PI 1.57079632679
#define EPSILON 0.00024414;

#define BLUR_RADIUS 15
#define TEXTURE_WIDTH 512

#define DX_SCALE 0.03f
#define DZ_SCALE 0.03f

Texture2D waveParticleHB1Texture : register(t0);
Texture2D waveParticleHB2Texture : register(t1);
SamplerState waveParticleSampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TexCoord;
};

struct PS_OUTPUT
{
    float4 col1 : SV_TARGET0;
    float4 col2 : SV_TARGET1;
};

float3 GetFilter(float v)
{
    float s, c;
    sincos(PI * v, s, c);
    return float3(
        0.5f * (c + 1.0f), // 0.5 ( cos(v) + 1 )
        -0.5f * s, // -0.5 sin(v)
        -0.25f * (c * c - s * s + c) // cos(2v) + cos(v)
    );
}

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
    output.col1 = float4(0, 0, 0, 0);
    output.col2 = float4(0, 0, 0, 0);

    float3 f123 = waveParticleHB1Texture.Sample(waveParticleSampler, input.texCoord).xyz;
    float4 f45v = waveParticleHB2Texture.Sample(waveParticleSampler, input.texCoord);
    float4 deviation = float4(f45v.x, 0, f123.x, 1); // initialize deviation at this pixel
    float4 gradient = float4(f123.y, 0, 0, 1); // initialize gradient at this pixel
    float2 gradCorr = float2(f123.z, f45v.y); // initialize gradient correction

    for (int i = 1; i <= BLUR_RADIUS; i++)
    {
        float offset = i / float(TEXTURE_WIDTH);

        float4 f123B = waveParticleHB1Texture.Sample(waveParticleSampler, input.texCoord + float2(0, offset));
        float4 f123T = waveParticleHB1Texture.Sample(waveParticleSampler, input.texCoord + float2(0, -offset));

        float4 f45vB = waveParticleHB2Texture.Sample(waveParticleSampler, input.texCoord + float2(0, offset));
        float4 f45vT = waveParticleHB2Texture.Sample(waveParticleSampler, input.texCoord + float2(0, -offset));

        float3 f = GetFilter(i / float(BLUR_RADIUS));

        deviation.x += (f45vB.x + f45vT.x) * f.x * f.x; // deviation X
        deviation.y += (f45vB.y - f45vT.y) * 2 * f.x * f.y; // deviation Y
        deviation.z += (f123B.x + f123T.x) * f.x; // deviation Z
        gradient.x += (f123B.y + f123T.y) * f.x; // gradient X
        gradient.y += (f123B.x - f123T.x) * f.y; // gradient Y
        gradCorr.x += (f123B.z + f123T.z) * f.x * f.x; // gradient X horizontal deviation
        gradCorr.y += (f45vB.y + f45vT.y) * f.z; // gradient Y horizontal deviation

    }
    gradCorr *= PI / BLUR_RADIUS;
    gradient.xy *= (PI / BLUR_RADIUS) / (1 + gradCorr);

    deviation.x *= DX_SCALE;
    deviation.y *= DZ_SCALE;
    output.col1 = float4(-deviation.x, deviation.z, -deviation.y, deviation.w);
    output.col2 = gradient;

    return output;
}