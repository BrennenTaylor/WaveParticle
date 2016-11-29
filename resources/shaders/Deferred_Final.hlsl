Texture2D DiffuseMap : register(t0);
Texture2D LightTexture : register(t1);
SamplerState AnisoSampler : register(s0);

cbuffer Transforms
{
    matrix WorldMatrix;
    matrix WorldViewMatrix;
    matrix WorldViewProjectionMatrix;
};

cbuffer MaterialProperties
{
    float3 SpecularAlbedo;
};

// Input/Output structs
struct VSInput
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORDS0;
};

struct VSOutput
{
    float4 PositionCS : SV_Position;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 ScreenPos : SV_Position;
    float2 TexCoord : TEXCOORD;
};

// Vertex Shader
VSOutput VSMain(in VSInput input)
{
    VSOutput output;
    // Calculate the clip-space position
    output.PositionCS = mul(input.Position, WorldViewProjectionMatrix);
    // Pass along the texture coordinate
    output.TexCoord = input.TexCoord;
    return output;
}

// Pixel Shader
float4 PSMain(in PSInput input) : SV_Target0
{
    // Sample the diffuse map
    float3 diffuseAlbedo = DiffuseMap.Sample(AnisoSampler, input.TexCoord).rgb;
    // Determin the indices for sampling the texture based on the current screen position
    int3 sampleIndices = int3(input.ScreenPos.xy, 0);
    // Sample the light target
    float4 lighting = LightTexture.Load(sampleIndices);
    // Apply the diffuse and specular albedo to the lighting value
    float3 diffuse = lighting.xyz * diffuseAlbedo;
    float3 specular = lighting.w * SpecularAlbedo;
    // Final output is sum of diffuse and specular
    return float4(diffuse + specular, 1.0f);
}
