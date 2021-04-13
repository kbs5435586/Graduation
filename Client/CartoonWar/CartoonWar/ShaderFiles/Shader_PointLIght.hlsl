#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vUV			: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4 vPointLight		: SV_TARGET;

};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	//vOut.vPosition	= float4(vIn.vPosition * 2.f, 1.f);
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vUV = vIn.vUV;
	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	vOut.vPointLight = vIn.vColor;

	return vOut;
}

