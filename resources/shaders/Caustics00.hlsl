#define PI 3.14159265359
#define HALF_PI 1.57079632679
#define EPSILON 0.00024414;

#define N 7
#define N_HALF 3

#define TEXTURE_WIDTH 512.0f

#define WATER_ROI 1.333f

Texture2D VB1Texture : register(t0);
Texture2D VB2Texture : register(t1);
SamplerState waveParticleSampler : register(s0);


static float3 groundPlaneBottomLeft = float3(-5.0f, -2.0f, -5.0f);
static float3 baseWaterPlaneBottomLeft = float3(-5.0f, 0.0f, -5.0f);
static float groundToPlaneHeight = 2.0f;

// Directional Light
static float3 lightDir = normalize(float3(2.0f * sqrt(3.0f), -2.0f, 0.0f));
// static float3 lightDir = normalize(float3(0.0f, -1.0f, 0.0f));

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TexCoord;
};

struct PS_OUTPUT
{
    float4 color0 : SV_TARGET0;
    float4 color1 : SV_TARGET1;
};

VS_OUTPUT VSMain(uint VertexID : SV_VertexID)
{
    VS_OUTPUT output;
    // Set texture coordinates
    // X: VertexID / 2 & 2
    // Y: VertexID & 2
    output.texCoord = float2( (VertexID << 1) & 2, VertexID & 2);
    output.position = float4(output.texCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
    return output;
}

struct Ray
{
    float3 origin;
    float3 direction;
};

struct Intersection
{
    float3 normal;
    float3 pt;
    bool valid;
    float t;
};

bool intersectPlane(float3 n, float3 p0, Ray ray, inout float t) 
{ 
    // assuming vectors are all normalized
    float denom = dot(n, ray.direction); 
    if (abs(denom) > 1e-6) { 
        float3 p0l0 = p0 - ray.origin; 
        t = dot(p0l0, n) / denom; 
        return (t >= 0.0); 
    }

    return false;
}

Intersection RayPlaneIntersection(float3 planeNormal, float3 pointOnPlane, Ray ray) 
{
    Intersection inter;

    float t = 0;
    if (intersectPlane(planeNormal, pointOnPlane, ray, t))
    {
        inter.pt = ray.origin + ray.direction * t;
        inter.valid = true;
        inter.t = t;
        return inter;
    }
    inter.valid = false;
    return inter;
}

PS_OUTPUT PSMain(VS_OUTPUT input)
{
    PS_OUTPUT output;
    output.color0 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.color1 = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float intensity[N];
    for (int idx0 = 0; idx0 < N; idx0++)
    {
        intensity[idx0] = 0.0f;
    }

    float3 baseWaterRefractedDirection = refract(lightDir, float3(0.0f, 1.0f, 0.0f), 1.0f / WATER_ROI);
    baseWaterRefractedDirection *= 1.0f;

    // output.color0 = float4(lightDir, 1.0f);
    // output.color1 = float4(baseWaterRefractedDirection, 1.0f);
    // return output;

    float2 groundPlaneTexCoord = input.texCoord;
    float3 groundPlanePosition = groundPlaneBottomLeft + float3(10.0f, 0.0f, 0.0f) * groundPlaneTexCoord.x
        + float3(0.0f, 0.0f, 10.0f) * groundPlaneTexCoord.y;

    // Y-Offsets
    float P_Gy[N];
    for (int idx1 = -N_HALF; idx1 <= N_HALF; idx1++)
    {
        P_Gy[idx1 + N_HALF] = groundPlanePosition.y + idx1 * (10.0f / TEXTURE_WIDTH);
    }

    // This is the P_C Point referenced in the paper
    float3 heightmapPos = groundPlanePosition + groundToPlaneHeight * (-baseWaterRefractedDirection);

    // output.color0 = float4(groundPlanePosition - heightmapPos, 1.0f);
    // output.color1 = float4(heightmapPos, 1.0f);
    // return output;

    float2 waterBaseTexCoord = float2((heightmapPos.x - (-5.0f)) / 10.0f, (heightmapPos.z - (-5.0f)) / 10.0f);
    // output.color0 = float4(groundPlaneTexCoord - waterBaseTexCoord, 0.0f, 1.0f);
    // output.color1 = float4(waterBaseTexCoord, groundPlaneTexCoord.y - waterBaseTexCoord.y, 1.0f);
    // return output;

    for (int i = -N_HALF; i <= N_HALF; i++)
    //int i = 0;
    {
        float2 waterSampleTexcoord = waterBaseTexCoord + float2((i / TEXTURE_WIDTH), 0.0f);

        float3 waterBasePosition = baseWaterPlaneBottomLeft + float3(10.0f, 0.0f, 0.0f) * waterSampleTexcoord.x
            + float3(0.0f, 0.0f, 10.0f) * waterSampleTexcoord.y;

        float3 displacement = VB1Texture.SampleLevel(waveParticleSampler, waterSampleTexcoord.xy, 0).xyz;
        float3 displacedHeightmapPos = waterBasePosition + displacement;

        float3 gradientTextureSample = VB2Texture.SampleLevel(waveParticleSampler, waterSampleTexcoord.xy, 0).xyz;
        float3 gradient = float3(0.0, 1.0, 0.0);
        gradient.x += gradientTextureSample.x;
        gradient.z += gradientTextureSample.z;
        gradient = normalize(gradient);

        Ray traceRay;
        traceRay.origin = displacedHeightmapPos;
        traceRay.direction = normalize(refract(lightDir, gradient, 1.0f / WATER_ROI));

        Intersection inter = RayPlaneIntersection(float3(0.0f, 1.0f, 0.0f), float3(0.0f, -2.0f, 0.0f), traceRay);

        // Else, we calculate
        float ax = max(0.0f, 1.0f - abs(groundPlanePosition.x - inter.pt.x));

        //int j = 3;
        for (int j = 0; j < N; j++)
        {
            float ay = max(0.0f, 1.0f - abs(P_Gy[j] - inter.pt.y));
            intensity[j] += ax * ay;

        }
    }


    output.color0 = float4(intensity[0], intensity[1], intensity[2], intensity[3]);
    output.color1 = float4(intensity[4], intensity[5], intensity[6], 0.0f);
    return output;
}
