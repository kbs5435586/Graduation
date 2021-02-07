#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float3	vViewPos	: POSITION;
	float4	vColor		: COLOR;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition	= mul(mul(float4(vIn.vPosition, 1.0f), matView), matProj);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vColor		= vIn.vColor;

	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	return vIn.vColor;
}

