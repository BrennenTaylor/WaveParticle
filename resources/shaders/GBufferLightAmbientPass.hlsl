// Textures
Texture2D DiffuseAlbedoTexture      : register(t0);

struct VSOut
{
    float4 position : SV_POSITION;
};


VSOut VSMain(uint VertexID : SV_VertexID)
{
    VSOut output;
    // Set texture coordinates
    // X: VertexID / 2 & 2
    // Y: VertexID & 2
    float2 texCoord = float2( (VertexID << 1) & 2, VertexID & 2);
    output.position = float4(texCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

// Lighting pixel Shader
float4 PSMain(in float4 screenPos : SV_Position) : SV_Target0
{
    float3 diffuseAlbedo;

    // Determine our indices for sampling texture based on screen pos
    int3 sampleIndices = int3(screenPos.xy, 0);

    diffuseAlbedo = DiffuseAlbedoTexture.Load(sampleIndices).xyz;

    float3 ambient = diffuseAlbedo * 0.1f;

    return float4(ambient, 1.0f);

}
