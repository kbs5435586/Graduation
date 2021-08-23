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
	float2 vTexUV		: TEXCOORD0;
};


struct PS_OUT
{
	float4		vBlurXTex	: SV_TARGET0;
	float4		vBlurYTex	: SV_TARGET1;
	float4		vBlightTex	: SV_TARGET2;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition = float4(vIn.vPosition * 2.f, 1.f);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}
PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT	vOut;

	float4 BrightColor = (float4)0.f;
	float4 FragColor = g_texture0.Sample(Sampler0, vIn.vTexUV);

	float brightness = dot(FragColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 0.1f)
		BrightColor = float4(FragColor.rgb, 1.0);


	{
		//float2	t = vIn.vTexUV;
		//float2	uv = (float2)0;

		//float	tu = 1.f / 1920.f;

		//float4 vBlurX = (float4)0.f;
		//float4 vBlurY = (float4)0.f;

		//for (int i = -6; i < 6; ++i)
		//{
		//	uv = t + float2(tu * i, 0);
		//	vOut.vBlurXTex += Weight[6 + i] * g_texture0.Sample(Sampler0, uv);
		//	//vBlurX += Weight[6 + i] * BrightColor;
		//}

		//uv = (float2)0;
		//float tv = 1.f / (1080.f / 2.f);
		//for (int j = -6; j < 6; ++j)
		//{
		//	uv = t + float2(0, tv * j);
		//	vOut.vBlurYTex += Weight[6 + j] * g_texture0.Sample(Sampler0, uv);
		//	//vBlurY += Weight[6 + j] * BrightColor;
		//}

		//vBlurX /= Total;
		//vBlurY /= Total;

		//vOut.vBlurXTex /= Total;
		//vOut.vBlurYTex /= Total;

		////vOut.vBlurXTex = vBlurX + vBlurY;

	}

	vOut.vBlightTex = BrightColor;
	return vOut;
}

