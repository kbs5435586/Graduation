
#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
	float2	vTexUV		: TEXCOORD;
	float3	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float4	vWeight		: BLENDWEIGHT;
	float4	vIndices	: BLENDINDICES;
};
struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
	float3	vWeight123	: TEXCOORD1;
	float3	vWeight456	: TEXCOORD2;
	float4	vWorldPos	: TEXCOORD3;
	float4	vNormal		: NORMAL;
};
struct PS_OUT
{
	float4 vTarget0 	: SV_TARGET0;
	float4 vTarget1		: SV_TARGET1;
	float4 vTarget2		: SV_TARGET2;
	float4 vTarget3		: SV_TARGET3;
	float4 vTarget5		: SV_TARGET5;
};

//{
//
//
//	//float4 GetInverseColor(float4 srcColor)
//	//{
//	//	return (float4(1.0f, 1.0f, 1.0f, 1.0f) - srcColor);
//	//}
//
//	//VS_OUT	VS_Main(VS_IN vIn)
//	//{
//	//	VS_OUT	vOut;
//
//	//	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
//	//	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
//	//	vOut.vTexUV0 = vIn.vTexUV;
//	//	vOut.vTexUV1 = vIn.vTexUV;
//	//	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWorld));
//
//	//	float3 vTempNormal = normalize(vOut.vNormal);
//	//	float4 vLightDir = vCamPos - float4(vIn.vPosition, 1.f);
//	//	vLightDir = normalize(vLightDir);
//
//	//	float factor = (dot(float4(vTempNormal, 0.f), vLightDir) + 1.f) * 3.f;
//
//	//	vOut.vWeight123 = float3(1.0f - clamp(abs(factor - 5.0f), 0.0f, 1.0f),
//	//		1.0f - clamp(abs(factor - 4.0f), 0.0f, 1.0f),
//	//		1.0f - clamp(abs(factor - 3.0f), 0.0f, 1.0f));
//
//	//	vOut.vWeight456 = float3(1.0f - clamp(abs(factor - 2.0f), 0.0f, 1.0f),
//	//		1.0f - clamp(abs(factor - 1.0f), 0.0f, 1.0f),
//	//		1.0f - clamp(factor, 0.0f, 1.0f));
//
//	//	return vOut;
//	//}
//
//	//PS_OUT	PS_Main(VS_OUT vIn)
//	//{
//	//	PS_OUT vOut = (PS_OUT)0;
//
//	//	float4	vColor123 = g_texture0.Sample(Sampler0, vIn.vTexUV0);
//	//	float4	vColor456 = g_texture1.Sample(Sampler0, vIn.vTexUV1);
//
//
//
//	//	// get weights
//	//	float4 weight123 = float4(vIn.vWeight123, 1.f);
//	//	float4 weight456 = float4(vIn.vWeight456, 1.f);
//
//	//	// blend colors
//	//	vColor123 = saturate(dot(GetInverseColor(vColor123), weight123));
//	//	vColor456 = saturate(dot(GetInverseColor(vColor456), weight456));
//
//	//	float4 vColor = saturate(vColor123 + vColor456);
//	//	vOut.vTarget0 = saturate(GetInverseColor(vColor));
//
//
//	//	return vOut;
//	//}
//}

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut = (VS_OUT)0;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	return vOut;
}