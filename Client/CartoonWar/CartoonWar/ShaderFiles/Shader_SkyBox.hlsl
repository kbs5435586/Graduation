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


VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition	= mul(float4(vIn.vPosition, 0.f), matWVP).xyww;
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}



float4 PS_Main(VS_OUT vIn) :SV_TARGET
{
	float4 vOut = (float4)0;

	vOut = g_textureCube.Sample(Sampler0, vIn.vTexUV);

	return vOut;
}

