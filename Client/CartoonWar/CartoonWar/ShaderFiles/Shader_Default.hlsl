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
	float4	vColor		: COLOR;
	float3	vViewPos	: POSITION;
};
struct PS_OUT
{
	float4 vTarget0			: SV_TARGET0;
	float4 vTarget1			: SV_TARGET1;
	float4 vTarget2			: SV_TARGET2;
	float4 vTarget3			: SV_TARGET3;
	float4 vTarget4			: SV_TARGET4;
	float4 vTarget5			: SV_TARGET5;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos = mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vColor = vIn.vColor;
	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	vOut.vTarget0 = vIn.vColor;
	vOut.vTarget2.xyz = vIn.vColor;
	vOut.vTarget5.xyz = vIn.vColor;
	return vOut;
}

