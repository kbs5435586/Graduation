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




VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = float4(vIn.vPosition * 2.f, 1.f);
	vOut.vTexUV = vIn.vTexUV;


	return vOut;
}

float4	PS_Main(VS_OUT vIn) : SV_Target
{
	float4	vDiffuseTex		= g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vShadeTex		= g_texture1.Sample(Sampler0, vIn.vTexUV);
	float4	vSpecularTex	= g_texture2.Sample(Sampler0, vIn.vTexUV);
	float4	vBlurTex		= g_texture3.Sample(Sampler0, vIn.vTexUV);

	int iNumBlurSample = 5;

	vBlurTex.xy /= (float)iNumBlurSample;
	int iCnt = 1;


	for (int i = iCnt; i < iNumBlurSample; ++i)
	{
		float4 BColor = g_texture0.Sample(Sampler0, vIn.vTexUV + vBlurTex.xy * (float)i);
		if (vBlurTex.a < vDiffuseTex.a + 0.04f)
		{
			vDiffuseTex += BColor;
			iCnt++;
		}
	}
	vDiffuseTex /= (float)iCnt;



	float4	vTexMerge = vDiffuseTex *vShadeTex + vSpecularTex;

	return  vTexMerge;
}

 