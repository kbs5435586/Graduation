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
	float4	vDiffuseTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vNormalTex	= g_texture1.Sample(Sampler0, vIn.vTexUV);
	float4	vShadeTex	= g_texture2.Sample(Sampler0, vIn.vTexUV);
	float4	vSpecTex	= g_texture3.Sample(Sampler0, vIn.vTexUV);
	float4	vReflectTex	= g_texture4.Sample(Sampler0, vIn.vTexUV);
	//float4	vUITex		= g_texture5.Sample(Sampler0, vIn.vTexUV);
	
	float4	vTexMerge = vDiffuseTex * vShadeTex + vSpecTex;

	return  vTexMerge;
}

