#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};
struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};
struct PS_OUT
{
	float4 vTarget2		: SV_TARGET2;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) : SV_TARGET
{
	PS_OUT vOut = (PS_OUT)0;


	//float4 vNormalInfo = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//float4 vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);
	//float4 vTotalShade;
	//for (int i = 0; i < iNumLight; ++i)
	//{
	//	vTotalShade += Calculate_Shade(i, vNormal);
	//}
	//vTotalShade.w = 1.f;
	//
	//vOut.vTarget2 = vTotalShade;

	return vOut;
}

