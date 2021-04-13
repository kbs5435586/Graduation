#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4 vDirection : SV_TARGET;

};
//g_texture0: NormalTarget
//g_texture1: positionTarget

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4		vNormal			= g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4		vWorldPos		= g_texture1.Sample(Sampler0, vIn.vTexUV);
	//AD_Light	tLight			= Calculate_Light_Upgrade(0, vNormal, vWorldPos);


//	vOut.vDirection = tLight.vDiffuse*tLight.vShade + tLight.vSpecular;
	//vOut.vDirection = vNormal;
	//vDiffuseTex* vShadeTex + vSpecTex + vPointLightTex;
	return vOut;
}

