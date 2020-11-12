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

Texture2D VB1Texture : register(t0);
Texture2D VB2Texture : register(t1);
SamplerState ParticleTextureSamplerState;

// For now, we simply project full texture map to entire grid
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    // No change here
    float3 newPos = input.position;
    float3 displacement = VB1Texture.SampleLevel(ParticleTextureSamplerState, input.uv, 0).xyz;
    newPos += displacement;

    float3 gradientTextureSample = VB2Texture.SampleLevel(ParticleTextureSamplerState, input.uv, 0).xyz;
    float3 gradient = float3(0.0, 1.0, 0.0);
    gradient.x += gradientTextureSample.x;
    gradient.z += gradientTextureSample.z;

    // float small = 1.0f / 1000.0f;

    // float2 leftSampleUV = float2(input.uv.x - small, input.uv.y);
    // float2 rightSampleUV = float2(input.uv.x + small, input.uv.y);

    // float2 topSampleUV = float2(input.uv.x, input.uv.y - small);
    // float2 bottomSampleUV = float2(input.uv.x, input.uv.y + small);

    // float leftHeight = ParticleTexture.SampleLevel(ParticleTextureSamplerState, leftSampleUV, 0).z;
    // float rightHeight = ParticleTexture.SampleLevel(ParticleTextureSamplerState, rightSampleUV, 0).z;

    // float topHeight = ParticleTexture.SampleLevel(ParticleTextureSamplerState, topSampleUV, 0).z;
    // float bottomHeight = ParticleTexture.SampleLevel(ParticleTextureSamplerState, bottomSampleUV, 0).z;


    // // NOTE: The 100 here is the grid size in the x and y direction. This allows us to calculate the positions required for the normal
    // float3 left = float3(newPos.x - 0.1, leftHeight, newPos.z);
    // float3 right = float3(newPos.x + 0.1, rightHeight, newPos.z);
    // float3 top = float3(newPos.x,  topHeight, newPos.z - 0.1);
    // float3 bottom = float3(newPos.x, bottomHeight, newPos.z + 0.1);

    // float3 xNorm =  right - left;
    // float3 zNorm =  bottom - top;

    // // NOTE: We dont need to project this because we dont do a world matrix transform on the grid
    // output.normal = normalize(cross(zNorm, xNorm));

    output.normal = gradient;

    output.positionWS = float4(newPos, 1.0);
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

    float diffuseFac = dot(L, norm); 
    bool isLight = diffuseFac > 0.0f; 

    float nDotL = saturate(dot(norm, L));

    float3 diffuseMat = float3(0.0f, 1.0f, 1.0f);
    float3 specMat = float3(1.0f, 1.0f, 1.0f);

    float3 diffuse = nDotL * diffuseMat * isLight;

    float3 v = reflect(L, norm);
    float3 toEye = normalize(input.positionWS - eyeWS);
    float specFactor = 8.0;
    float specAmount = pow(max(dot(v, toEye), 0.0), specFactor);

    float3 specular = specAmount * specMat * isLight;

    return float4(diffuse + specular, 1.0f);
}
