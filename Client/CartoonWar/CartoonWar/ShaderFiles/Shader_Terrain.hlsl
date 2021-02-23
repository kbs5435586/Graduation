#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float3 vViewPos		: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD;
};

struct PS_OUT
{
	float4 vTarget0		: SV_TARGET0;
	float4 vTarget1		: SV_TARGET1;
	float4 vTarget2		: SV_TARGET2;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal,0.f), matWV).xyz);
	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float3 vInNormal = vIn.vNormal;
	float4 vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);


	LIGHT  tLight = Calculate_Light(0, vInNormal, vIn.vViewPos);

	vOut.vTarget0 = vOutColor;
	vOut.vTarget1.xyz = vIn.vNormal;
	vOut.vTarget2.xyz = (tLight.vSpecular.xyz) + (tLight.vAmbient.xyz * vOutColor.xyz);

	return vOut;
}

