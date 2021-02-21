#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float3 vTexUV		: TEXCOORD0;
	float4 vWorldPos	: TEXCOORD1;
};

struct PS_OUT
{
	float4 vTarget0		: SV_TARGET0;
	float4 vTarget3		: SV_TARGET3;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition	= mul(float4(vIn.vPosition, 0.f), matWVP).xyww;
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	vOut.vTarget0 = g_textureCube.Sample(Sampler0, vIn.vTexUV);
	vOut.vTarget3 = g_textureCube.Sample(Sampler0, vIn.vTexUV);

	return vOut;
}

