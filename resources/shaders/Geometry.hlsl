#ifndef GEOMETRY_HLSL
#define GEOMETRY_HLSL

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

Intersection RaySphereIntersection(float3 rayPt, float3 rayDir, float3 spherePos, float radius)
{
    Intersection isect;
    isect.valid = false;
    isect.pt = float3(0.0f, 0.0f, 0.0f);
    isect.normal = float3(0.0f, 1.0f, 0.0f);

    rayPt -= spherePos;
    rayPt /= radius;

    float A = dot(rayDir, rayDir);
    float B = 2.0f * dot(rayDir, rayDir);
    float C = dot(rayPt, rayPt) - 1.0f;
    float disc = B * B - 4.0f * A * C;

    if (disc < 0.0f)
    {
        return isect;
    }

    float t = (-B - sqrt(disc)) / (2.0f * A);

    // This looks like an issue...
    if (t < 0.0f)
    {
        t = (-B + sqrt(disc)) / (2.0f * A);
    }

    if (t >= 0.0f)
    {
        float3 p = float3(rayPt + t * rayDir);
        isect.valid = true;
        isect.normal = normalize(p);

        p *= radius;
        p += spherePos;
        isect.pt = p;
        isect.t = length(p - rayPt);
    }

    return isect;
}

bool intersectPlane(float3 n, float3 p0, Ray ray, inout float t) 
{ 
    // assuming vectors are all normalized
    float denom = dot(n, ray.direction); 
    if (denom > 1e-6) { 
        float3 p0l0 = p0 - ray.origin; 
        t = dot(p0l0, n) / denom; 
        return (t >= 0); 
    } 
 
    return false; 
} 

bool RayDiskIntersection(float3 norm, float3 pointOnDisk, float radius, Ray ray) 
{ 
    float t = 0; 
    if (intersectPlane(norm, pointOnDisk, ray, t)) { 
        float3 p = ray.origin + ray.direction * t; 
        float3 v = p - pointOnDisk; 
        float d2 = dot(v, v); 
        return (sqrt(d2) <= radius); 
     } 
     return false; 
} 

#endif