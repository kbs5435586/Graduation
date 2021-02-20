#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}

float4	PS_Main(VS_OUT vIn) : SV_Target
{
	float4	vTex0 = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vTex1 = g_texture1.Sample(Sampler0, vIn.vTexUV);
	float4	vTex2 = g_texture2.Sample(Sampler0, vIn.vTexUV);
	float4	vTex3 = g_texture3.Sample(Sampler0, vIn.vTexUV);
	float4	vTex4 = g_texture4.Sample(Sampler0, vIn.vTexUV);

	float4	vTexMerge = vTex0 * vTex3 + vTex4;

	return vTexMerge;
}

