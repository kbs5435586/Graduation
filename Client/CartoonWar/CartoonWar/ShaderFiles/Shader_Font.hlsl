#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition		: POSITION;
	float2	vTexUV			: TEXCOORD0;
};
struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float2	vTexUV				: TEXCOORD0;
};
VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}

//float4	vOut = (float4)0;
//float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);
//float	fPersent = g_int_0 / 100.f;
//
//if (vIn.vTexUV.x >= fPersent)
//{
//	vOut = float4(0.f, 0.f, 0.f, 1.f);
//}
//else
//vOut = vDiffuse;
//
//
//return vOut;

float4 PS_Main(VS_OUT vIn) :SV_TARGET
{
	float4 vOut = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4 vBlack = float4(0.f,0.f,0.f,0.f);
	if (vOut.r == 0.0f)
	{
		vOut.a = 0.0f;
	}

	else
	{
		vOut.a = 1.0f;
		vOut = vOut;
	}
	vOut.rgb = vOut.rgb * 0.f;
	return vOut;
}
