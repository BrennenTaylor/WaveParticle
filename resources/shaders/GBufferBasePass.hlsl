cbuffer Transforms : register(b0)
{
    float4x4 WorldMatrix;
    float4x4 WorldViewMatrix;
    float4x4 WorldViewProjMatrix;
};

cbuffer MatProperties : register(b1)
{
    float3 SpecularAlbedo;
    float SpecularPower;
};

// Textures and Samplers
Texture2D DiffuseMap                : register(t0);
SamplerState AnisoSampler           : register(s0);

// Input and Output Structures
struct VSInput
{
    float3 Position                 : POSITION;
    float2 TexCoord                 : TEXCOORDS0;
    float3 Normal                   : NORMAL;
};

struct VSOutput
{
    float4 PositionCS               : SV_Position;
    float2 TexCoord                 : TEXCOORD;
    float3 NormalWS                 : NORMALWS;
    float3 PositionWS               : POSITIONWS;
};

struct PSInput
{
    float4 PositionSS               : SV_Position;
    float2 TexCoord                 : TEXCOORD;
    float3 NormalWS                 : NORMALWS;
    float3 PositionWS               : POSITIONWS;
};

struct PSOutput
{
    float4 Normal                   : SV_Target0;
    float4 DiffuseAlbedo            : SV_Target1;
    float4 SpecularAlbedo           : SV_Target2;
    float4 Position                 : SV_Target3;
};

// G-buffer vertex shader
VSOutput VSMain(in VSInput input)
{
    VSOutput output;

    // Convert position and normals to world space
    output.PositionWS = mul(float4(input.Position, 1.0f), WorldMatrix).xyz;
    output.NormalWS = normalize(mul(input.Normal, (float3x3)WorldMatrix));

    // Calculate clip space projection
    output.PositionCS = mul(float4(input.Position, 1.0f), WorldViewProjMatrix);

    // Pass along texture coords
    output.TexCoord = input.TexCoord;

    return output;
}


// G-buffer pixel shader
PSOutput PSMain(in PSInput input)
{
    PSOutput output;

    // Sample the diffuse map
    //float3 diffuseAlbedo = DiffuseMap.Sample(AnisoSampler, input.TexCoord).rgb;
    float3 diffuseAlbedo = float3(1.0f, 0.0f, 0.0f);

    // Normalize the normal after interpolation
    float3 normalWS = normalize(input.NormalWS);

    // Output G-buffer values
    output.Normal = float4(normalWS, 1.0f);
    output.DiffuseAlbedo = float4(diffuseAlbedo, 1.0f);
    output.SpecularAlbedo = float4(SpecularAlbedo, SpecularPower);
    output.Position = float4(input.PositionWS, 1.0f);

    return output;
}
