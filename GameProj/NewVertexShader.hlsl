cbuffer constantbuffer : register(b0)
{
	float4x4 mvp;
}

struct VS_Input
{
	float3 pos: POSITION;
	float3 norm: NORMAL;
	float2 uv: TEXCOORD;
	float3 col: COLOR;
};

struct VS_Output
{
	float4 pos: SV_POSITION;
	float2 uv: TEXCOORD;
};

VS_Output main(VS_Input input)
{
	VS_Output output;
	
	//output.pos = mul(mvp, float4(input.pos, 1.0f));
	output.pos = float4(input.pos, 1.0f);
	output.uv = input.uv;
	return output;
}