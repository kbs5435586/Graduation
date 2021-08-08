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
	float4 vColor;
	float4 vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//vDiffuse = g_texture1.Sample(Sampler0, vIn.vTexUV);
	float PI = 3.141592;
	// PI + 1, ´ë·« -2.14
	   
	//if (g_int_0 == 1)
	//{
	//	if (vIn.vTexUV.x <= 0.1f || vIn.vTexUV.x >= 0.9f || vIn.vTexUV.y <= 0.1f || vIn.vTexUV.y >= 0.9f)
	//	{
	//
	//		vColor = float4(1.f, 1.f, 1.f, 1.f);
	//	}
	//	else
	//	{
	//		vColor = vDiffuse;
	//	}
	//}
	//else if (g_int_0 == 2)
	//{
	//	if (vIn.vTexUV.x <= 0.1f || vIn.vTexUV.x >= 0.9f || vIn.vTexUV.y <= 0.1f || vIn.vTexUV.y >= 0.9f)
	//	{
	//
	//		vColor = float4(1.f, 0.f, 0.f, 1.f);
	//	}
	//	else
	//	{
	//		vColor = vDiffuse;
	//	}
	//}
	//else
	//		vColor = vDiffuse;

	//if (vIn.vTexUV.x <= 0.05f || vIn.vTexUV.x >= 0.95f || vIn.vTexUV.y <= 0.05f || vIn.vTexUV.y >= 0.95f)
	//{
	//
	//	vColor = g_texture1.Sample(Sampler0, vIn.vTexUV);
	//}
	//else
	//{
	//	vColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//}
	if (vIn.vTexUV.x <= 0.1f || vIn.vTexUV.x >= 0.9f || vIn.vTexUV.y <= 0.1f || vIn.vTexUV.y >= 0.9f)
	{

		vColor = g_texture1.Sample(Sampler0, vIn.vTexUV);
	}
	else
	{
		vColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
	}

	

	return  vColor;
}

