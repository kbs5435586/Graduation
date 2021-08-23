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
	float4	vLightTex		= g_texture1.Sample(Sampler0, vIn.vTexUV);
	float4	vSpecularTex	= g_texture2.Sample(Sampler0, vIn.vTexUV);
	float4	vVelocityTex	= g_texture3.Sample(Sampler0, vIn.vTexUV);
	float4	vPointTex		= g_texture4.Sample(Sampler0, vIn.vTexUV);
	float4	vBloomTex		= g_texture5.Sample(Sampler0, vIn.vTexUV);

	float4	vTexMerge		= (float4)0;
	int		iNumBlurSample	= 25;

	vVelocityTex.xy /= (float)iNumBlurSample;
	int iCnt = 1;
	for (int i = iCnt; i < iNumBlurSample; ++i)
	{
		float4 BColor = g_texture0.Sample(Sampler0, vIn.vTexUV + vVelocityTex.xy * (float)i);
		if (vVelocityTex.a < vDiffuseTex.a + 1.f)
		{
			vDiffuseTex += BColor;
			iCnt++;
		}
	}
	vDiffuseTex /= (float)iCnt;




	 //vTexMerge = vDiffuseTex * (vLightTex + vPointTex + vBloomTex) + vSpecularTex ;
	vTexMerge = vDiffuseTex * (vLightTex + vPointTex ) + vSpecularTex ;
	return  vTexMerge;
}

 