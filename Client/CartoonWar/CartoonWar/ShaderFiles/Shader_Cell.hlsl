#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
};

struct VS_OUT
{
	float4 vPosition		: SV_POSITION;
	float4 vColor			: COLOR;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vWorldPos		: TEXCOORD1;
};

struct PS_OUT
{
	float4 vTarget0 : SV_TARGET0;

};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition	= mul(mul(float4(vIn.vPosition, 1.0f), matView), matProj);
	vOut.vWorldPos	= mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vColor		= vIn.vColor;

	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	vOut.vTarget0 = vIn.vColor;

	return vOut;
}

