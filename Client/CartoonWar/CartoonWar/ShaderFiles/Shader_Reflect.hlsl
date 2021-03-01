#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
	float3  vNormal		: NORMAL;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float4	vColor		: COLOR;
	float4  vNormal		: NORMAL;
	float4	vReflectionPosition : TEXCOORD0;
};
struct PS_OUT
{
	float4 vReflectTex			: SV_TARGET4;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vPosition = mul(vOut.vPosition, matView);
	vOut.vPosition = mul(vOut.vPosition, matProj);


	vOut.vColor = vIn.vColor;

	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	vOut.vReflectTex = vIn.vColor;
	return vOut;
}

