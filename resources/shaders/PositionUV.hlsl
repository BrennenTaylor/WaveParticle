struct VS_INPUT
{
    float3 worldPos : POSITION;
    float2 worldDir : TEXCOORD;
    float amplitude : AMPLITUDE;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 velocity : VELOCITY;
    float amplitude : AMPLITUDE;
};

PS_INPUT VSMain(VS_INPUT input)
{
	PS_INPUT output;
    // Note: No transform by camera, we want stuff splatted out in world space
    // Note: This y position can probably be changed
	output.position = float4(input.worldPos.x, input.worldPos.y, 0.5, 1.0);
	output.velocity = input.worldDir;
    output.amplitude = input.amplitude;
	return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return float4(input.velocity, input.amplitude, 1.0);
}
