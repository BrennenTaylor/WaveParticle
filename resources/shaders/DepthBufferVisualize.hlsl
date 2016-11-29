struct VSOut
{
    float4 position : SV_POSITION;
    float2 texCoord : TexCoord;
};

Texture2D FullscreenTexture;
SamplerState FullscreenSamplerState;

VSOut VSMain(uint VertexID : SV_VertexID)
{
    VSOut output;
    // Set texture coordinates
    // X: VertexID / 2 & 2
    // Y: VertexID & 2
    output.texCoord = float2( (VertexID << 1) & 2, VertexID & 2);
    output.position = float4(output.texCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

float LinearizeDepth(float color)
{
    float zNear = 1.0f;
    float zFar = 1000.0f;
    float depth = color;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
    //return depth;
}

float4 PSMain(VSOut input) : SV_TARGET
{
    float color = FullscreenTexture.Sample(FullscreenSamplerState, input.texCoord).r;
    float c = LinearizeDepth(color);
    //float
    return float4(c, c, c, 1.0f);
}
