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
	float amplitude : AMPLITUDE;
};

Texture2D MainTexture;
SamplerState MainTextureSamplerState;

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
    float3 newPos = input.position;
    float3 readSample = MainTexture.SampleLevel(MainTextureSamplerState, input.uv, 0).xyz;
    newPos.y = readSample.x;
    //if (newPos.y > 1.0f)
    //{
    //    newPos.y = 1.0f;
    //}
	output.amplitude = newPos.y;
	output.position = mul(float4(newPos, 1.0f), WVP);
    output.color = input.color;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	return (1.0f - input.amplitude) * float4(1.0f, 1.0f, 1.0f, 1.0f) + (input.amplitude * float4(0.0f, 0.0f, 1.0f, 1.0f));
}
