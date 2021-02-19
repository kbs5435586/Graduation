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
	float3	vViewPos			: POSITION;
	float2	vTexUV				: TEXCOORD0;
	float4	vReflectPosition	: TEXCOORD1;
};
struct PS_OUT
{
	float4 vTarget5				: SV_TARGET5;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matReflect);
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matProj);
	vOut.vViewPos = mul(float4(vIn.vPosition, 1.f), matWV).xyz;

	vOut.vTexUV = vIn.vTexUV;

	matrix matReflectProjWorld;

	matReflectProjWorld = mul(matReflect, matProj);
	matReflectProjWorld = mul(matWorld, matReflectProjWorld);

	vOut.vReflectPosition = mul(vIn.vPosition, matReflectProjWorld);
	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;


	float2 reflectTexCoord;
	reflectTexCoord.x = vIn.vReflectPosition.x / vIn.vReflectPosition.w / 2.0f + 0.5f;
	reflectTexCoord.y = -vIn.vReflectPosition.y / vIn.vReflectPosition.w / 2.0f + 0.5f;

	// Sample the texture pixel from the reflection texture using the projected texture coordinates.
	//reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
	vOut.vTarget5 = g_texture0.Sample(Sampler0, reflectTexCoord);

	return vOut;
}

