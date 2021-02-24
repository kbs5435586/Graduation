#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition			: POSITION;
	float2	vTexUV				: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float2	vTexUV				: TEXCOORD0;
	float4  vWorldPos			: TEXCOORD1;
	float4	vReflectPosition	: TEXCOORD2;
};
struct PS_OUT
{
	float4 vTarget5				: SV_TARGET5;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vTexUV = vIn.vTexUV;

	//matrix matReflectProjWorld;
	//
	//matReflectProjWorld = mul(matReflect, matProj);
	//matReflectProjWorld = mul(matWorld, matReflectProjWorld);
	//
	//vOut.vReflectPosition = mul(vIn.vPosition, matReflectProjWorld);
	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;


	//float2 reflectTexCoord;
	////reflectTexCoord.x = vIn.vReflectPosition.x / vIn.vReflectPosition.w / 2.0f + 0.5f;
	////reflectTexCoord.y = -vIn.vReflectPosition.y / vIn.vReflectPosition.w / 2.0f + 0.5f;
	//reflectTexCoord.x = vIn.vTexUV.x
	//reflectTexCoord.y = vIn.vTexUV.y

	vOut.vTarget5 = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//vOut.vTarget5 = g_texture0.Sample(Sampler0, reflectTexCoord);
	//vOut.vTarget5 =  float4(1.f,0.f,0.f,1.f);

	return vOut;
}

