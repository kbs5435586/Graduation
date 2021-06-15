#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3 vPosition	: POSITION;
	float2 vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
};

struct PS_OUT
{
	float4 vDiffuseTex		: SV_TARGET0;
	float4 vNormalTex		: SV_TARGET1;
	float4 vPositionTex		: SV_TARGET2;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);

	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;


	vOut.vDiffuseTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	vOut.vNormalTex  = g_texture0.Sample(Sampler0, vIn.vTexUV);
	vOut.vPositionTex = g_texture0.Sample(Sampler0, vIn.vTexUV);

	return vOut;
}

