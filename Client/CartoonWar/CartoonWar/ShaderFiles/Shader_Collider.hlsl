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
};
struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vDiffuseTex_			: SV_TARGET1;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;
	matrix matTemp = g_arrFinalBoneMat[2];
	matTemp = mul(matTemp, matWorld);

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vPosition = mul(vOut.vPosition, matView);
	vOut.vPosition = mul(vOut.vPosition, matProj);
	vOut.vColor = vIn.vColor;
	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	//vOut.vDiffuseTex = float4(1.f,1.f,1.f,1.f);
	vOut.vDiffuseTex_ = float4(1.f,1.f,1.f,1.f);
	return vOut;
}

