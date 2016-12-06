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
	float3 normal : NORMALS;
    float amplitude : AMPLITUDE;
};

Texture2D MainTexture;
SamplerState MainTextureSamplerState;

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
    float3 newPos = input.position;
    float3 readSample = MainTexture.SampleLevel(MainTextureSamplerState, input.uv, 0).xyz;
    newPos.y = readSample.x * 5;

	float small = 1.0f/500.0f;

    float2 leftUV = float2(input.uv.x - small, input.uv.y);
    float2 rightUV = float2(input.uv.x + small, input.uv.y);

    float2 topUV = float2(input.uv.x, input.uv.y - small);
    float2 bottomUV = float2(input.uv.x, input.uv.y + small);

	float3 leftCol = MainTexture.SampleLevel(MainTextureSamplerState, leftUV, 0).xyz;
	float3 rightCol = MainTexture.SampleLevel(MainTextureSamplerState, rightUV, 0).xyz;

	float3 topCol = MainTexture.SampleLevel(MainTextureSamplerState, topUV, 0).xyz;
    float3 bottomCol = MainTexture.SampleLevel(MainTextureSamplerState, bottomUV, 0).xyz;

	float3 left = float3(leftUV.x * 100, leftCol.x*5, leftUV.y * 100);
	float3 right = float3(rightUV.x * 100, rightCol.x*5, rightUV.y * 100);
	float3 top = float3(topUV.x * 100, topCol.x*5, topUV.y * 100);
	float3 bottom = float3(bottomUV.x * 100, bottomCol.x*5, bottomUV.y * 100);

	float3 xNorm =  right - left;
	float3 zNorm =  bottom - top;
	output.normal = normalize(cross(zNorm, xNorm));

	// Calculate normals
	//output.normal = float3(0.0f, 1.0f, 0.0f);

	output.amplitude = newPos.y;

	output.position = mul(float4(newPos, 1.0f), WVP);
    //output.normal = mul(float4(output.normal, 1.0f), WVP);
    //output.normal = normalize(output.norm);
    output.color = input.color;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	float3 norm = normalize(input.normal);

	return float4(norm, 1.0f);
	//return (1.0f - input.amplitude) * float4(1.0f, 1.0f, 1.0f, 1.0f) + (input.amplitude * float4(0.0f, 0.0f, 1.0f, 1.0f));
}
