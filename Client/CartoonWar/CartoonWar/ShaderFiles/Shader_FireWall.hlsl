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
	float2 vTexCoord1	: TEXCOORD1;
	float2 vTexCoord2	: TEXCOORD2;
	float2 vTexCoord3	: TEXCOORD3;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;


	vOut.vTexCoord1 = (vOut.vTexUV * vScale.x);
	vOut.vTexCoord1.y = vOut.vTexCoord1.y - (fFrameTime* vScrollSpeed.x);

	vOut.vTexCoord2 = (vOut.vTexUV * vScale.y);
	vOut.vTexCoord2.y = vOut.vTexCoord2.y - (fFrameTime * vScrollSpeed.y);

	vOut.vTexCoord3 = (vOut.vTexUV * vScale.z);
	vOut.vTexCoord3.y = vOut.vTexCoord3.y - (fFrameTime * vScrollSpeed.z);
	return vOut;
}


float4 PS_Main(VS_OUT vIn) : SV_TARGET
{

	float4	vNoise1;
	float4	vNoise2;
	float4	vNoise3;


	float4	vFinalNoise;

	float	fPertrub;

	float2	vNoiseCoord;
	float4	vFireColor;
	float4	vAlphaColor;

	
	float4 vNoise = g_texture2.Sample(Sampler0, vIn.vTexCoord1);


	vNoise1 = g_texture2.Sample(Sampler0, vIn.vTexCoord1);
	vNoise2 = g_texture2.Sample(Sampler0, vIn.vTexCoord2);
	vNoise3 = g_texture2.Sample(Sampler0, vIn.vTexCoord3);

	vNoise1 = (vNoise1 - 0.5f) * 2.0f;
	vNoise2 = (vNoise2 - 0.5f) * 2.0f;
	vNoise3 = (vNoise3 - 0.5f) * 2.0f;

	vNoise1.xy = vNoise1.xy * fDistortion1.xy;
	vNoise2.xy = vNoise2.xy * fDistortion2.xy;
	vNoise3.xy = vNoise3.xy * fDistortion3.xy;


	vFinalNoise = vNoise1 + vNoise2 + vNoise3;
	//
	fPertrub = ((1.0f - vIn.vTexUV.y) * fDistortionScale) + fDistortionBias;
	
	vNoiseCoord.xy = (vFinalNoise.xy * fPertrub) + vIn.vTexUV.xy;
	vFireColor = g_texture1.Sample(Sampler1, vNoiseCoord.xy);
	
	vAlphaColor = g_texture0.Sample(Sampler1, vNoiseCoord.xy);
	
	vFireColor.a = vAlphaColor;
	
	
	//if (vIn.vTexUV.y < 0.4999f)
	//	vFireColor = float4(1.f, 0, 0, 1.f);
	//else
	//	vFireColor = float4(0, 0, 0, 1.f);



	//vFireColor = float4(1.f, 0, 0, 1.f);
	return vFireColor;
}

