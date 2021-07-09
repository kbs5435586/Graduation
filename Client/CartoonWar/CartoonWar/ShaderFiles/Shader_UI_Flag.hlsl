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
	float PI = 3.141592;
	// PI + 1, ´ë·« -2.14
	if (g_int_0)
	{
		if (1 - atan2(vIn.vTexUV.x - 0.5f, vIn.vTexUV.y - 0.5f) > (g_float_0 * 0.2 * PI) - 2.14)
			vIn.vTexUV = float2(0.f, 0.f);
	}
	if (g_int_1)
	{
		if (1 - atan2(vIn.vTexUV.x - 0.5f, vIn.vTexUV.y - 0.5f) > (g_float_0 * 0.2 * PI) - 2.14)
			vIn.vTexUV = float2(0.f, 0.f);
	}
	
	return g_texture0.Sample(Sampler0, vIn.vTexUV);
}