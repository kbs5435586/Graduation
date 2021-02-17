#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3 vPosition	: POSITION;
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float4 vViewPos		: POSITION;
	float2 vTexUV		: TEXCOORD;
};

struct PS_OUT
{
	float4 vTarget0		: SV_TARGET0;
	float4 vTarget1		: SV_TARGET1;
	float4 vTarget2		: SV_TARGET2;
	
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV);

	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;


	//float4 vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
	vOut.vTarget0 = g_texture0.Sample(Sampler0, vIn.vTexUV);
	vOut.vTarget1 = g_texture0.Sample(Sampler0, vIn.vTexUV);
	vOut.vTarget2 = vIn.vViewPos;

	return vOut;
}

