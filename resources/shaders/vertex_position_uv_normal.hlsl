cbuffer cbPerObject
{
	float4x4 WVP;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};


struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

Texture2D MainTexture;
SamplerState MainTextureSamplerState;

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
	output.position = mul(float4(input.position, 1.0f), WVP);
	output.uv = input.uv;
	output.normal = input.normal;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	// Normalize normal after interpolation
    input.normal = normalize(input.normal);

    input.uv = float2(input.uv.x, input.uv.y);
    //float4 diffuse = MainTexture.Sample(MainTextureSamplerState, input.uv);
	float4 diffuse = float4(1.0f, 0.0f, 0.0f, 1.0f);

    float3 finalColor;

    float3 lightDir = float3(-1.0f, 1.0f, 1.0f);

    float3 ambientConst = float3(0.1f, 0.1f, 0.1f);
    float3 diffuseConst = float3(2.0f, 2.0f, 2.0f);

    finalColor = diffuse * ambientConst;
    finalColor += saturate(dot(lightDir, input.normal) * diffuseConst * diffuse);

	return float4(finalColor, diffuse.a);
	//return finalColor;
}
