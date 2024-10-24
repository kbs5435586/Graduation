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


VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}


float4 PS_Main(VS_OUT vIn) : SV_TARGET
{
	float4	vDiffuseTex = g_texture0.Sample(Sampler0, vIn.vTexUV);

	//return g_texture0.Sample(Sampler0, vIn.vTexUV);

	//float4	vTexMerge = vDiffuseTex + vShadeTex * vPointTex;
	float4	vTexMerge = vDiffuseTex;
	return vTexMerge;
}

