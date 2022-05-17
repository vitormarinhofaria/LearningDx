#pragma pack_matrix(row_major)
cbuffer constantbuffer : register(b1)
{
    float4x4 mvp;
    float3 light;
    float4x4 model;
    float4x4 invert;
}

cbuffer lightBuffer : register(b0)
{
    float3 lightPosition;
    float3 lightColor;
    float3 ambientLightColor;
}

struct VS_Output
{
	float4 pos: SV_POSITION;
	float3 norm: NORMAL;
	float2 uv: TEXCOORD;
    float3 currentPos : CURRENTPOS;
};

Texture2D    mytexture : register(t0);
SamplerState mysampler : register(s0);

float4 main(VS_Output input) : SV_TARGET
{
    float3 normal = normalize(input.norm);
    float3 lightDirection = normalize(lightPosition - input.currentPos);
    float lightDistance = mul(lightPosition - input.currentPos, lightDirection);
    float diffuse = max(dot(normal, lightDirection), 0.0) * 8 / lightDistance;
 
    return mytexture.Sample(mysampler, input.uv) * float4(lightColor, 1.0f) * (diffuse+ 0.2);
//	return float4(1.0, 0.5, 0.2, 1.0);
}