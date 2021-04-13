#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition			: POSITION;
	float4	vColor				: COLOR;
	float3  vNormal				: NORMAL;
};
struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float4	vColor				: COLOR;
	float4  vNormal				: NORMAL;
	float4	vWorldPos			: TEXCOORD1;
	float4  vProjPos			: TEXCOORD2;
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

	//vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);

	// -0.5f, 0.5f, 0.f,1.f
	vOut.vColor = vIn.vColor;
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWorld));
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vProjPos = vOut.vPosition;

	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;

	AD_Light	tLight_Default = Calculate_Light_Upgrade(0, vIn.vNormal, vIn.vWorldPos);
	AD_Light	tLight_Point = Calculate_Light_Upgrade(1, vIn.vNormal, vIn.vWorldPos);


	vOut.vDiffuseTex = vIn.vColor;
	vOut.vNormalTex  = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;


	//vOut.vShadeTex = tLight_Point.vShade + tLight_Default.vShade;
	//vOut.vSpecularTex = tLight_Point.vSpecular + tLight_Default.vSpecular;
	//vOut.vPointLightTex = tLight_Point.vDiffuse;

	return vOut;
}

