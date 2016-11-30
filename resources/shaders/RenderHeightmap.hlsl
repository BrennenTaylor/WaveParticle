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
	float3 normal : NORMALS;
};

Texture2D MainTexture;
SamplerState MainTextureSamplerState;

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
    float3 newPos = input.position;
    float3 readSample = MainTexture.SampleLevel(MainTextureSamplerState, input.uv, 0).xyz;

	float small = 0.1f;

    newPos.y = readSample.x * 5;

	float leftCol = MainTexture.SampleLevel(MainTextureSamplerState, float2(input.uv.x - small, input.uv.y), 0).x;
	float rightCol = MainTexture.SampleLevel(MainTextureSamplerState, float2(input.uv.x + small, input.uv.y), 0).x;

	float topCol = MainTexture.SampleLevel(MainTextureSamplerState, float2(input.uv.x, input.uv.y - small), 0).x;
	float bottomCol = MainTexture.SampleLevel(MainTextureSamplerState, float2(input.uv.x, input.uv.y + small), 0).x;

	float3 left = float3(newPos.x - 1.0f, topCol, newPos.z);
	float3 right = float3(newPos.x + 1.0f, topCol, newPos.z);
	float3 top = float3(newPos.x, topCol, newPos.z + 1.0f);
	float3 bottom = float3(newPos.x, topCol, newPos.z - 1.0f);

	float3 xNorm = left - right;
	float3 zNorm = top - bottom;
	output.normal = normalize(cross(xNorm, zNorm));

	// Calculate normals
	//output.normal = float3(0.0f, 1.0f, 0.0f);

	output.amplitude = newPos.y;
	output.position = mul(float4(newPos, 1.0f), WVP);
    output.color = input.color;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	float3 norm = normalize(input.normal);

	//return float4(norm, 1.0f);
	return (1.0f - input.amplitude) * float4(1.0f, 1.0f, 1.0f, 1.0f) + (input.amplitude * float4(0.0f, 0.0f, 1.0f, 1.0f));
}
