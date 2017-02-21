cbuffer cbPerObject
{
	float4x4 WVP;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float amplitude : AMPLITUDE;
    float angle : ANGLE;
    float3 origin : ORIGIN;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
    float amplitude : AMPLITUDE;
    float angle : ANGLE;
};

Texture2D MainTexture;
SamplerState MainTextureSamplerState;

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;

    float4x4 transformToOrigin = 
    {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -input.origin.x, -input.origin.y, -input.origin.z, 1.0
    };

    // y
    // float4x4 directionRotation =
    // {
    //     cos(input.angle), 0.0, sin(input.angle), 0.0,
    //     0.0, 1.0, 0.0, 0.0,
    //     -sin(input.angle), 0.0, cos(input.angle), 0.0,
    //     0.0, 0.0, 0.0, 1.0
    // };

    // z
    float4x4 directionRotation =
    {
        cos(input.angle), -sin(input.angle), 0.0, 0.0,
        sin(input.angle), cos(input.angle), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    // float4x4 transformFromOrigin = 
    // {
    //     1.0, 0.0, 0.0, input.origin.x,
    //     0.0, 1.0, 0.0, input.origin.y,
    //     0.0, 0.0, 1.0, input.origin.z,
    //     0.0, 0.0, 0.0, 1.0
    // };

    float4x4 finalRot =   directionRotation;

    float4 temp = mul(float4(input.position, 1.0), finalRot);

	output.position = mul(temp, WVP);
	output.uv = input.uv;
    output.amplitude = input.amplitude;
    output.angle = input.angle;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float dist = distance(input.uv, float2(0.5f, 0.5f));
    float x = 3.1415926f * 2.0f * dist;
    float alpha = 0.5f * (cos(x) + 1);
    if (dist > 0.4f)
    {
        alpha = 0.0;
    }
    // float4 color = float4(1.0f, input.amplitude, 1.0f, alpha);
    float4 color = float4(1.0f, input.amplitude, 1.0f, alpha);
    //if (color.r == 0.0f)
    //{
    //    color.a = 0.0f;
    //}
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
    return color;
}
