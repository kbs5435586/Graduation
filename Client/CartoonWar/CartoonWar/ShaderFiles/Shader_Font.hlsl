#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition	= mul(mul(float4(vIn.vPosition, 1.0f), matView), matProj);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}

float4	PS_Main(VS_OUT vIn):SV_TARGET
{
	float4 color;
	
	
	color = g_texture0.Sample(Sampler0, vIn.vTexUV);
	if (color.r == 0.0f)
	{
		color.a = 0.0f;
	}
	else
	{
		color.rgb =float3(1.f,0.f,0.f);
		color.a = 1.0f;
	}
	
	return color;
}

