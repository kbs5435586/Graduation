#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3 vPosition	: POSITION;
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float3 vViewPos		: POSITION;
	float2 vTexUV		: TEXCOORD;
};

struct PS_OUT
{
	float4 vTarget0 : SV_Target0; // Diffuse
	float4 vTarget1 : SV_Target1; // Normal
	float4 vTarget2 : SV_Target2; // Position
};


VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos = mul(float4(vIn.vPosition, 1.f), matWV).xyz;

	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}


float4 PS_Main(VS_OUT vIn): SV_TARGET
{
	//PS_OUT vOut = (PS_OUT)0;
	
	//vIn.vTexUV = float2(vIn.vTexUV.xy * 2.f - 1.f);


	float4 fOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//vOut.vTarget1 = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//vOut.vTarget2 = g_texture0.Sample       (Sampler0, vIn.vTexUV);

	

	return fOutColor;

}

