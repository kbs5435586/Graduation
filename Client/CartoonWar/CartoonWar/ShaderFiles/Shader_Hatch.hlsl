
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
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};
struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
	float4 vDepthTex			: SV_TARGET3;
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

	if (g_int_0 == 1)
		Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal, vIn.vWeight, vIn.vIndices, 0);


	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vNormal	= mul(float4(vIn.vNormal, 0.f), matWV);
	vOut.vWorldPos	= mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vTexUV		= vIn.vTexUV;
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), matWV).xyz);
	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWV).xyz);


	//float3 normalW = normalize(mul(vIn.vNormal, (float3x3)matRev));
	float4	vNormal = normalize(float4(vIn.vNormal,0.f));

	float  diffuse = min(1.0, max(0, dot(normalize(-tLight[0].vLightDir.xyz), vNormal)));
	float  hatchFactor = diffuse * 6.f;
	float3 weight0 = 0.0;
	float3 weight1 = 0.0;

	if (hatchFactor > 5.0)
	{
		weight0.x = 1.0;
	}
	else if (hatchFactor > 4.0)
	{
		weight0.x = 1.0 - (5.0 - hatchFactor);
		weight0.y = 1.0 - weight0.x;
	}
	else if (hatchFactor > 3.0)
	{
		weight0.y = 1.0 - (4.0 - hatchFactor);
		weight0.z = 1.0 - weight0.y;
	}
	else if (hatchFactor > 2.0)
	{
		weight0.z = 1.0 - (3.0 - hatchFactor);
		weight1.x = 1.0 - weight0.z;
	}
	else if (hatchFactor > 1.0)
	{
		weight1.x = 1.0 - (2.0 - hatchFactor);
		weight1.y = 1.0 - weight1.x;
	}
	else if (hatchFactor > 0.0)
	{
		weight1.y = 1.0 - (1.0 - hatchFactor);
		weight1.z = 1.0 - weight1.y;
	}
	vOut.vWeight123 = weight0;
	vOut.vWeight456 = weight1;

	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float3 vViewNormal = vIn.vNormal;
	if (g_int_2)
	{

		float3 vTSNormal = g_texture8.Sample(Sampler0, vIn.vTexUV).xyz;
		vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
		float3x3 matTBN = { vIn.vTangent, vIn.vBinormal, vIn.vNormal.xyz };
		vViewNormal = normalize(mul(vTSNormal, matTBN));
	}

	float4 vHatvhTex0 = g_texture0.Sample(Sampler0, vIn.vTexUV*2.f) * vIn.vWeight123.x;
	float4 vHatvhTex1 = g_texture1.Sample(Sampler0, vIn.vTexUV*2.f) * vIn.vWeight123.y;
	float4 vHatvhTex2 = g_texture2.Sample(Sampler0, vIn.vTexUV*2.f) * vIn.vWeight123.z;
	float4 vHatvhTex3 = g_texture3.Sample(Sampler0, vIn.vTexUV*2.f) * vIn.vWeight456.x;
	float4 vHatvhTex4 = g_texture4.Sample(Sampler0, vIn.vTexUV*2.f) * vIn.vWeight456.y;
	float4 vHatvhTex5 = g_texture5.Sample(Sampler0, vIn.vTexUV*2.f) * vIn.vWeight456.z;

	float4 hatchColor = vHatvhTex0 + vHatvhTex1 + vHatvhTex2 + vHatvhTex3 + vHatvhTex4 + vHatvhTex5;

	float	fDepth = vIn.vWorldPos.z / 450.f;

	float4 vDiffuse = g_texture9.Sample(Sampler0, vIn.vTexUV);
	//vOut.vDiffuseTex = hatchColor;
	vOut.vDiffuseTex = hatchColor;
	vOut.vNormalTex = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	vOut.vDepthTex = float4(fDepth, fDepth, fDepth, 1.f);

	return vOut;
}