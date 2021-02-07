#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float3	vViewPos	: POSITION;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);

	return vOut;
}

float4	PS_Main(VS_OUT vIn) : SV_Target
{
	return float4(1.f,1.f,1.f,1.f);
}

