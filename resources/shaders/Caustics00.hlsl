#define PI 3.14159265359
#define HALF_PI 1.57079632679
#define EPSILON 0.00024414;

#define N 7
#define N_HALF 3

#define TEXTURE_WIDTH 512

Texture2D VB1Texture : register(t0);
SamplerState waveParticleSampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TexCoord;
};

struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0;
    float4 color1 : SV_TARGET1;
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
    output.color0 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.color1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return output;
    // float intensity[N];
    // for (int i = 0; i < N; i++)
    // {
    //     intensity[i] = 0.0f;
    // }

    // float P_Gy[N];
    // for (int i = -N_HALF; i <= N_HALF; i++)
    // {
    //     P_Gy[i] = input.texCoord.y + i * (1.0 / TEXTURE_WIDTH);
    // }


    // float2 texCoord = input.texCoord;

    // float3 bottomLeft = float3(-5.0f, 0.0f, -5.0f);

    // float3 heightmapPos = bottomLeft + float3(10.0f, 0.0f, 0.0f) * texCoord.x + float3(0.0f, 0.0f, 10.0f) * texCoord.y;
    // float3 displacement = VB1Texture.SampleLevel(ParticleTextureSamplerState, input.uv, 0).xyz;
    // float3 centerPos = 


    // float3 velAmp = waveParticleTexture.Sample(waveParticleSampler, texCoord).xyz;
    // float4 f123 = float4(velAmp.z, 0, -1.0 * 0.5 * velAmp.z, 1);
    // float4 f45v = float4(0, velAmp.z, sign(velAmp.z) * velAmp.xy);

    // for (int i = 1; i <= floor(BLUR_RADIUS); i++)
    // {
    //     // Offset should be number of pixels that
    //     float offset = i / float(TEXTURE_WIDTH);
    //     float4 velAmpL = waveParticleTexture.Sample(waveParticleSampler, texCoord + float2(offset, 0));
    //     float4 velAmpR = waveParticleTexture.Sample(waveParticleSampler, texCoord + float2(-offset, 0));
    //     float ampSum = velAmpL.z + velAmpR.z;
    //     float ampDif = velAmpL.z - velAmpR.z;
    //     float3 f = GetFilter(i / BLUR_RADIUS);
    //     f123.x += ampSum * f.x;
    //     f123.y += ampDif * f.y;
    //     f123.z += ampSum * f.z;
    //     f45v.x += ampDif * f.x * f.y * 2;
    //     f45v.y += ampSum * f.x * f.x;

    //     f45v.z += (sign(velAmpL.z) * velAmpL.x + sign(velAmpR.z) * velAmpR.x) * f.x;
    //     f45v.w += (sign(velAmpL.z) * velAmpL.y + sign(velAmpR.z) * velAmpR.y) * f.x;
    // }

    // output.col1 = f123;
    // output.col2 = f45v;
    // return output;
}
