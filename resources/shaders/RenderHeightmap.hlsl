#define PI 3.14159265359
#define HALF_PI 1.57079632679
#define EPSILON 0.00024414;

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
    float4 color : COLOR;
    float4 positionWS : POSITIONWS;
    float4 position : SV_POSITION;
    float3 normal : NORMALS;
};

struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0;
    float4 color1 : SV_TARGET1;
};

// Directional Light
static float3 lightColor = float3(0.0f, 1.0f, 1.0f);
static float3 lightDir = normalize(float3(2.0f * sqrt(3.0f), -2.0f, 0.0f));
// static float3 lightDir = normalize(float3(0.0f, -1.0f, 0.0f));

static float3 baseWaterPlaneBottomLeft = float3(-5.0f, 0.0f, -5.0f);

Texture2D VB1Texture : register(t0);
Texture2D VB2Texture : register(t1);
SamplerState ParticleTextureSamplerState;

// For now, we simply project full texture map to entire grid
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    // No change here
    float3 newPos = input.position;
    // float3 displacement = VB1Texture.SampleLevel(ParticleTextureSamplerState, input.uv, 0).xyz;
    // newPos += displacement;

    float3 gradientTextureSample = VB2Texture.SampleLevel(ParticleTextureSamplerState, input.uv, 0).xyz;
    float3 gradient = float3(0.0, 1.0, 0.0);
    gradient.x += gradientTextureSample.x * 10.0f;
    gradient.z += gradientTextureSample.y * 10.0f;
    output.normal = normalize(gradient);

    // float small = 1.0f / 1000.0f;

    // float2 leftSampleUV = float2(input.uv.x - small, input.uv.y);
    // float2 rightSampleUV = float2(input.uv.x + small, input.uv.y);

    // float2 topSampleUV = float2(input.uv.x, input.uv.y - small);
    // float2 bottomSampleUV = float2(input.uv.x, input.uv.y + small);

    // float leftHeight = VB1Texture.SampleLevel(ParticleTextureSamplerState, leftSampleUV, 0).y;
    // float rightHeight = VB1Texture.SampleLevel(ParticleTextureSamplerState, rightSampleUV, 0).y;

    // float topHeight = VB1Texture.SampleLevel(ParticleTextureSamplerState, topSampleUV, 0).y;
    // float bottomHeight = VB1Texture.SampleLevel(ParticleTextureSamplerState, bottomSampleUV, 0).y;


    // // NOTE: The 100 here is the grid size in the x and y direction. This allows us to calculate the positions required for the normal
    // float3 left = float3(newPos.x - 0.1, leftHeight, newPos.z);
    // float3 right = float3(newPos.x + 0.1, rightHeight, newPos.z);
    // float3 top = float3(newPos.x,  topHeight, newPos.z - 0.1);
    // float3 bottom = float3(newPos.x, bottomHeight, newPos.z + 0.1);

    // float3 xNorm =  right - left;
    // float3 zNorm =  bottom - top;

    // // NOTE: We dont need to project this because we dont do a world matrix transform on the grid
    // output.normal = normalize(cross(zNorm, xNorm));

    output.positionWS = float4(newPos, 1.0);
    output.position = mul(float4(newPos, 1.0f), WVP);
    output.color = input.color;
    return output;
}

static float waterRoughness = 0.1f;
static float waterSpec = 1.0f;

float F_Shlick(float NdotL, float Kr) //Shlick Approximation = Kr + (1 - Kr) * (1 - (N.V))^5
{
    return Kr + (1.0 - Kr) * pow((1.0 - NdotL), 5.0);
}

float D_TrowbridgeReitz(float NH2, float roughness2) //Trowbridge-Reitz = m^2 / (1 - m^2) * cos^2(alpha) - 1
{
    float denom = NH2 * roughness2 + (1.0 - NH2);
    return roughness2 / (PI * pow(denom, 2.0));
}

float G_GGXSmith(float roughness2, float NdotV, float NdotL)
{
    float g1 = (NdotL * 2.0) / (NdotL + sqrt(roughness2 + (1.0 - roughness2) * pow(NdotL, 2.0)));
    float g2 = (NdotV * 2.0) / (NdotV + sqrt(roughness2 + (1.0 - roughness2) * pow(NdotV, 2.0)));
    return g1 * g2;
}


PS_OUTPUT PSMain(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    output.color0 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.color1 = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Height mapping from black to white
    // {
    // // float3 offset = float3(0.1f, 0.1f, 0.1f);

    // // float3 white = float3(1.0f, 1.0f, 1.0f);
    // // float3 black = float3(0.0f, 0.0f, 0.0f);

    // // float maxHeight = 1.0f;
    // // float minHeight = 0.0f;

    // // float readHeight = input.positionWS.y;

    // // float amountWhite  = (readHeight - minHeight) / (maxHeight - minHeight);

    // // float3 finalColor = white * amountWhite + black * (1.0f - amountWhite);
    // // finalColor += offset;

    // // return float4(finalColor, 1.0);

    // }



    // {
    
    // // float3 N = normalize(input.normal);

    // // // Directional Light
    // // float3 L = -lightDir;

    // // float nDotL = saturate(dot(N, L));

    // // float3 diffuseMat = float3(0.0f, 1.0f, 1.0f);
    // // float3 specMat = float3(1.0f, 1.0f, 1.0f);

    // // float3 diffuse = nDotL * diffuseMat * lightColor;


    // // float3 V = normalize(input.positionWS - eyeWS);
    // // float3 H = normalize(L + V);

    // // float specFactor = 8.0;
    // // float specular = pow(saturate(dot(N, H)), specFactor) * lightColor * specMat * nDotL;

    // // return float4(diffuse + specular, 1.0f);
    // }

    float3 L = -lightDir;
    
    float3 N = normalize(input.normal);
    float3 V = normalize(eyeWS - input.positionWS.xyz);

    // output.color1 = float4(V, 0.0f);
    // return output;


    float NdotV = max(dot(N, V), 0.0f);
    float3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0f);
    float NdotH = max(dot(N, H), 0.0f);
    float VdotH = max(dot(V, H), 0.0f);

    float NH2 = pow(NdotH, 2.0f);
    float tan2Alpha = (NH2 - 1.0) / NH2;
    float roughness2 = pow(clamp(waterRoughness, 0.01, 0.99), 2.0);
    float Kr = pow((1.0 - 1.33) / (1.0 + 1.33), 2.0);


    // return float4(NdotV, NdotL, NdotH, VdotH);

    float F = F_Shlick(NdotV, Kr);
    float D = D_TrowbridgeReitz(NH2, roughness2);
    float G = G_GGXSmith(roughness2, NdotV, NdotL);
    float3 specular = waterSpec * (lightColor * ((F * G * D) / ((PI * NdotV))));
    float3 lambertian = (1.0 - waterRoughness) * (lightColor * NdotL);

    float3 diffuse = lambertian; //float3(0.0f, 0.0f, 0.0f);
    float3 CookTorrance = specular;

    float3 materialAlbedo = float3(6 / 255.0f, 66 / 255.0f, 115 / 255.0f);

    float3 finalColor = materialAlbedo * (diffuse + CookTorrance);

    output.color0 = float4(finalColor, 1.0f);
    return output;
}
