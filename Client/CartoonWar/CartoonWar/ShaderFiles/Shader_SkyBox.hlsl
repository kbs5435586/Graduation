#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float3 vViewPos		: POSITION;
	float3 vTexUV		: TEXCOORD;
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
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;

	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

//PS_OUT PS_Main(VS_OUT vIn)
//{
//	PS_OUT vOut = (PS_OUT)0;
//
//	vOut.vTarget0 = g_texture7.Sample(Sampler0, vIn.vTexUV);
//	vOut.vTarget1.xyz = vIn.vViewPos;
//	vOut.vTarget2 = g_texture7.Sample(Sampler0, vIn.vTexUV);
//	return vOut;
//}


float4 PS_Main(VS_OUT vIn) : SV_TARGET
{

	float4 vTarget = g_texture7.Sample(Sampler0, vIn.vTexUV);
	return vTarget;
}

