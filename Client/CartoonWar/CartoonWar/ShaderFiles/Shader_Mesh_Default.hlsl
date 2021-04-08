#include "Value.hlsl"
#include "Function.hlsl"




struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
	float2	vTexUV		: TEXCOORD;
	float3	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float4	vWeight		: BLENDWEIGHT;
	float4	vIndices	: BLENDINDICES;
};
struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float4	vNormal		: NORMAL;
	float4	vTangent	: TANGENT;
	float4	vBinormal	: BINORMAL;
	float2	vTexUV		: TEXCOORD0;
	float4  vWorldPos	: TEXCOORD1;
};
struct PS_OUT
{
	float4	vDiffuseTex			: SV_TARGET0;
	float4	vNormalTex			: SV_TARGET1;
	float4	vShadeTex			: SV_TARGET2;
	float4	vSpecularTex		: SV_TARGET3;
	float4	vPointLightTex		: SV_TARGET5;
	float4	vPositionTex		: SV_TARGET6;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);

	vOut.vTexUV =  vIn.vTexUV;
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWVP));
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), matWVP));
	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWVP));
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);

	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	AD_Light	tLight_Default = Calculate_Light_Upgrade(0, vIn.vNormal, vIn.vWorldPos);
	AD_Light	tLight_Point = Calculate_Light_Upgrade(1, vIn.vNormal, vIn.vWorldPos);


	vOut.vDiffuseTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	vOut.vNormalTex  = vIn.vNormal;
	vOut.vShadeTex = tLight_Point.vShade + tLight_Default.vShade;
	vOut.vSpecularTex = tLight_Point.vSpecular + tLight_Default.vSpecular;
	vOut.vPointLightTex = tLight_Point.vDiffuse;
	vOut.vPositionTex = vIn.vWorldPos;
	return vOut;
}

