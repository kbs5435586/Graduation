#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition		: POSITION;
	float3	vNormal			: NORMAL;
	float2	vTexUV			: TEXCOORD0;
	float3	vBiNormal		: BINORMAL;
	float3	vTangent		: TANGENT;
};
struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float4	vNormal				: NORMAL;
	float2	vTexUV				: TEXCOORD0;
	float4	vWorldPos			: TEXCOORD1;
	float4	vBinormal			: BINORMAL;
	float4	vTangent				: TANGENT;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
};


VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal, 0.f), matWV));
	vOut.vWorldPos	= mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vBinormal	= normalize(mul(float4(vIn.vBiNormal, 0.f), matWV));
	vOut.vTangent	= normalize(mul(float4(vIn.vTangent, 0.f), matWV));
	vOut.vTexUV		= vIn.vTexUV;
	return vOut;
}


PS_OUT PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut;

	vIn.vTexUV.x += fFrameTime;
	float4 vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV*30.f);
	float4	vNormal = mul(vIn.vNormal, matViewInv);


	float3 vTSNormal = g_texture1.Sample(Sampler0, vIn.vTexUV * 30.f).xyz;
	vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;

	float3x3 matTBN = { vIn.vTangent.xyz, vIn.vBinormal.xyz, vIn.vNormal.xyz };
	vNormal = float4(normalize(mul(vTSNormal, matTBN)), 0.f);


	vOut.vDiffuseTex= vDiffuse;
	vOut.vNormalTex= vNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	return vOut;
}

