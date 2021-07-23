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
	float4 vDepthTex			: SV_TARGET3;
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

float4 PS_Main(VS_OUT vIn) :SV_TARGET
{
	float4	vOut;
	float	fDepth = vIn.vWorldPos / 300.f;
	float4	vWorldPos = mul(vIn.vWorldPos, matViewInv);

	float2 vScreenUV = float2(vIn.vPosition.x / 1920, vIn.vPosition.y / 1080);
	float2 vDir = normalize(float2(0.5f, 0.5f) - vScreenUV);
	float fDist = distance(float2(0.5f, 0.5f), vScreenUV);

	float fRatio = (fDist / 0.5f);
	fRatio = fRatio * (0.2f + (fRatio) * 0.4f);

	vIn.vTexUV += vDir * sin(abs((fRatio - g_fAccTime * 0.16f) * 40.f)) * 0.03f;
	vIn.vTexUV.x += fFrameTime;
	float4 vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);

	return vDiffuse;
}
