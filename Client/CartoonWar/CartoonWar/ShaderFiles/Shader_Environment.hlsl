#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float3	vNormal		: NORMAL;
	float2	vTexUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float3	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float2	vTexUV		: TEXCOORD0;
	float4  vWorldPos	: TEXCOORD1;
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

	vOut.vPosition	= mul(mul(float4(vIn.vPosition, 1.0f), matView), matProj);
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal, 0.f), matWorld));
	vOut.vTangent   = normalize(mul(float4(vIn.vTangent, 0.f), matWorld));
	vOut.vBinormal	= normalize(mul(float4(vIn.vBinormal, 0.f), matWorld));
	vOut.vWorldPos	= mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vNormal = float4((vIn.vNormal.xyz * 2.f) - 1.f, 0.f);

	vOut.vDiffuseTex = float4(0.f,0.f,0.f,1.f);
	vOut.vNormalTex  = vNormal;
	vOut.vShadeTex	 = Calculate_Shade(vNormal);

	return vOut;
}

