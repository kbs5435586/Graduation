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


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition	= mul(mul(float4(vIn.vPosition, 1.0f), matView), matProj);
	vOut.vWorldPos	= mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vColor		= vIn.vColor;

	return vOut;
}

float4	PS_Main(VS_OUT vIn):SV_TARGET
{
	float4 vOut = (float4)0;
	vOut = float4(0.f,0.f,1.f,1.f);

	return vOut;
}

