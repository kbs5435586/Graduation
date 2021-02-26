#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition			: POSITION;
	float2	vTexUV				: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float2	vTexUV				: TEXCOORD0;
	float4  vWorldPos			: TEXCOORD1;
	float4	vReflectPosition	: TEXCOORD2;
};
struct PS_OUT
{
	float4 vReflectTex			: SV_TARGET4;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	vOut.vReflectTex = float4(0.f,0.f,1.f,1.f);
	return vOut;
}

