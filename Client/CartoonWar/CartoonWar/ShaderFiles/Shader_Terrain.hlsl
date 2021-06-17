#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition		: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD;
};

struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float4	vNormal				: NORMAL;
	float2	vTexUV				: TEXCOORD0;
	float4	vWorldPos			: TEXCOORD1;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}



//PS_OUT PS_Main(VS_OUT vIn)
//{
//	PS_OUT vOut = (PS_OUT)0;
//
//	AD_Light	tLight_Default = Calculate_Light_Upgrade(0, vIn.vNormal, vIn.vWorldPos);
//	AD_Light	tLight_Point_Red = Calculate_Light_Upgrade(1, vIn.vNormal, vIn.vWorldPos);
//
//	float4	vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
//	float4	vFogColor = float4(0.5f, 0.5f, 0.5f, 1.f);
//
//	vOut.vDiffuseTex = vOutColor;
//	vOut.vNormalTex = vIn.vNormal;
//	vOut.vPositionTex = vIn.vWorldPos;
//	vOut.vShadeTex = tLight_Point_Red.vShade + tLight_Default.vShade;
//	vOut.vPointLightTex = tLight_Point_Red.vDiffuse;
//
//
//	return vOut;
//}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	AD_Light tCol = (AD_Light)0;
	float4	vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV*30.f);

	vOut.vDiffuseTex = vOutColor;
	vOut.vNormalTex = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	return vOut;
}
