#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float3	vNormal		: NORMAL;
	float2	vTexUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV0		: TEXCOORD0;
	float2	vTexUV1		: TEXCOORD1;
	float3	vWeight123	: TEXCOORD2;
	float3	vWeight456	: TEXCOORD3;
	float4	vWorldPos	: TEXCOORD4;
	float4	vNormal		: NORMAL;


};
struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vShadeTex			: SV_TARGET2;
	float4 vSpecularTex			: SV_TARGET3;
	float4 vPointLightTex		: SV_TARGET5;
	float4 vPositionTex			: SV_TARGET6;
};

float4 GetInverseColor(float4 srcColor)
{
	return (float4(1.0f, 1.0f, 1.0f, 1.0f) - srcColor);
}

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vTexUV0 = vIn.vTexUV;
	vOut.vTexUV1 = vIn.vTexUV;
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWorld));



	return vOut;
}

PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	{
		float4 vLightDir = tLight[1].vLightPos - vIn.vPosition;
		//float4 vLightDir =  vIn.vPosition- tLight[1].vLightPos;
		vLightDir = normalize(vLightDir);

		float factor = max((dot(vIn.vNormal, -vLightDir) + 1.0f) * 2.f, 0.f);
		//float factor = max(dot(-vLightDir, normalize(vIn.vNormal)), 0.f);


		float3 vWeight123 = float3(
			1.0f - clamp(abs(factor - 5.0f), 0.0f, 1.0f),
			1.0f - clamp(abs(factor - 4.0f), 0.0f, 1.0f),
			1.0f - clamp(abs(factor - 3.0f), 0.0f, 1.0f));


		float3 vWeight456 = float3(
			1.0f - clamp(abs(factor - 2.0f), 0.0f, 1.0f),
			1.0f - clamp(abs(factor - 1.0f), 0.0f, 1.0f),
			1.0f - clamp(factor, 0.0f, 1.0f));



		float4	vColor123 = g_texture0.Sample(Sampler0, vIn.vTexUV0);
		float4	vColor456 = g_texture1.Sample(Sampler0, vIn.vTexUV1);



		// get weights
		float4 weight123 = float4(vWeight123, 1.0);
		float4 weight456 = float4(vWeight456, 1.0);

		// blend colors
		vColor123 = saturate(dot(GetInverseColor(vColor123), weight123));
		vColor456 = saturate(dot(GetInverseColor(vColor456), weight456));


		float4 vColor = saturate(vColor123 + vColor456);
		vOut.vDiffuseTex = saturate(GetInverseColor(vColor));
		vOut.vNormalTex = saturate(GetInverseColor(vColor));
		vOut.vShadeTex = saturate(GetInverseColor(vColor));
		vOut.vSpecularTex = saturate(GetInverseColor(vColor));
	}
	
	//vOut.vDiffuseTex = g_texture0.Sample(Sampler0, vIn.vTexUV0);
	//vOut.vNormalTex = g_texture0.Sample(Sampler0, vIn.vTexUV0);
	//vOut.vShadeTex = g_texture0.Sample(Sampler0, vIn.vTexUV0);
	//vOut.vPositionTex = vIn.vWorldPos;

	return vOut;
}

