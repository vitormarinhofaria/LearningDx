#pragma pack_matrix(row_major)
cbuffer constantbuffer : register(b1)
{
	float4x4 mvp;
    float3 light;
    float4x4 model;
    float4x4 invert;
}

struct VS_Input
{
	float3 pos: POS;
	float3 norm: NORM;
	float2 uv: TEX;
    
};

struct VS_Output 
{
	float4 pos: SV_POSITION;
	float3 norm: NORMAL;
	float2 uv: TEXCOORD;
    float3 currentPos : CURRENTPOS;
};

VS_Output main( VS_Input input)
{
	VS_Output output;
    output.currentPos = mul(float4(input.pos, 1.0), model);
	//output.pos = mul(float4(input.pos, 1.0f), mvp);
    output.pos = mul(float4(input.pos, 1.0), mvp);
    float3 Normal = mul(float4(input.norm, 0), invert);
	output.norm = Normal;
	output.uv = input.uv;
	return output;
}