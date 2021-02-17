#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float3	vViewPos	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4	vTarget0	: SV_TARGET0;
	float4	vTarget1	: SV_TARGET1;
	
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos = mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	tLight.vLightDir;


	float4	vNormalTex = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//float4	vNormal = vector(vNormalTex.xyz * 2.f - 1.f, 0.f);
	//
	//vOut.vTarget0 = max(dot(normalize(tLight.vLightDir) * -1.f, vNormal), 0.f);
	//vOut.vTarget1 = max(dot(normalize(tLight.vLightDir) * -1.f, vNormal), 0.f);
	
	vOut.vTarget0 = float4(vIn.vViewPos, 1.f);
	vOut.vTarget1 = vNormalTex;
	return vOut;
}

