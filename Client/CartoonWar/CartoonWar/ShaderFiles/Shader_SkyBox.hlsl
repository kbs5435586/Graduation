#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float3 vViewPos		: POSITION;
	float3 vTexUV		: TEXCOORD;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;

	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

float4 PS_Main(VS_OUT vIn) : SV_Target
{

	return g_texture0.Sample(Sampler0, vIn.vTexUV);
}

