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
	float4	I_vPosition : POSITION;
	float4	vColor		: COLOR;
	float4  vNormal		: NORMAL;
};
struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vShadeTex			: SV_TARGET2;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	//vOut.I_vPosition = mul(float4(vIn.vPosition, 1.f), I_matWVP);
	vOut.vColor = vIn.vColor;
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWorld));
	return vOut;
}


PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vNormal = float4((vIn.vNormal.xyz * 2.f) - 1.f, 0.f);

	vOut.vDiffuseTex = vIn.vColor;
	vOut.vNormalTex  = vNormal;
	vOut.vShadeTex	 = Calculate_Shade(vNormal);
	//vOut.vUITex = vIn.I_vPosition;
	return vOut;
}

