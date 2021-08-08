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
	float2 vTexUV		: TEXCOORD;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}


float4 PS_Main(VS_OUT vIn) : SV_TARGET
{
	float4	vOut = (float4)0;
	float4	vRed = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vBlue = g_texture1.Sample(Sampler0, vIn.vTexUV);
	float	fPersent = g_int_0 / 10.f;

	if (g_int_1 == 0 )		//	RED
	{	
		if (vIn.vTexUV.x >= fPersent)
		{
			vOut = float4(0.f, 0.f, 0.f, 1.f);
		}
		else
			vOut = vRed;
	}
	else if (g_int_1 == 1)	// BLUE
	{
		if (vIn.vTexUV.x >= fPersent)
		{
			vOut = float4(0.f, 0.f, 0.f, 1.f);
		}
		else
			vOut = vBlue;
	}


	return vOut;
}
