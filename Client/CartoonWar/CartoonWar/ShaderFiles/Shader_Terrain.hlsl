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
	float4	vPosition			: SV_POSITION;
	float4	vNormal				: NORMAL;
	float2	vTexUV				: TEXCOORD0;
	float4	vWorldPos			: TEXCOORD1;
	float4	vBinormal			: BINORMAL;
	float4	vTangent			: TANGENT;
	float4	vProjPos			: TEXCOORD2;
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
	VS_OUT vOut = (VS_OUT)0;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vProjPos = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));

	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWV));
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), matWV));
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}



PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4	vPosition = mul(vIn.vWorldPos, matViewInv);
	float4	vNormal = mul(vIn.vNormal, matViewInv);
	
	float4	vDiffuse;
	float3	vTSNormal;
	float4	vViewNormal = vIn.vNormal;
	float4	vFillterTex = g_texture8.Sample(Sampler1, vIn.vTexUV);
	if (vFillterTex.r == 1.f)
	{
		vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV * 30.f);

	}
	else if (vFillterTex.g == 0.f)
	{
		vDiffuse = g_texture1.Sample(Sampler0, vIn.vTexUV * 30.f);

	}
	
	if (vIn.vTexUV.x < 0.005f || vIn.vTexUV.x > 1.f - 0.004f)
	{
		float2 vTempUV = vIn.vTexUV;
		vTempUV.x *= 1500.f;
		vTempUV.y *= 100.f;
		vDiffuse = g_texture2.Sample(Sampler0, vTempUV);
	}
	else if (vIn.vTexUV.y < 0.004f || vIn.vTexUV.y > 1.f - 0.004f)
	{
		float2 vTempUV = vIn.vTexUV;
		vTempUV.y *= 1500.f;
		vTempUV.x *= 100.f;
		vDiffuse = g_texture2.Sample(Sampler0, vTempUV);
	}



	float4	vBrushTex = (float4)0;
	if (vBrushPos.x - fBrushRange < vPosition.x && vPosition.x <= vBrushPos.x + fBrushRange &&
		vBrushPos.z - fBrushRange < vPosition.z && vPosition.z <= vBrushPos.z + fBrushRange)
	{
		float2		vTexUV;

		vTexUV.x = (vPosition.x - (vBrushPos.x - fBrushRange)) / (fBrushRange * 2.f);
		vTexUV.y = ((vBrushPos.z + fBrushRange) - vPosition.z) / (fBrushRange * 2.f);

		vBrushTex = g_texture9.Sample(Sampler0, vTexUV);
	}


	float4	vLightDir = -normalize(tLight[0].vLightDir);
	float	fDot = saturate(dot(vNormal, vLightDir));
	fDot = (ceil(fDot * 3.f) / 3.f);
	float4	vMtrlDif = float4(fDot, fDot, fDot, 0.f);
	float4	vMtrlAmb = float4(0.3f, 0.3f, 0.3f, 0.f);
	float3	fRimColor = float3(-2.f, -2.f, -2.f);
	float4	vView = normalize(vCamPos - vPosition);
	float	fRim = saturate(dot(vNormal, vView));
	float	fRimPower = 30.f;
	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);
	float	fDepth = vIn.vWorldPos.z / 450.f;


	vOut.vDiffuseTex = (vDiffuse + vMtrlEmiv) + vBrushTex;
	//vOut.vDiffuseTex = (vDiffuse ) + vBrushTex;
	vOut.vNormalTex = vViewNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	vOut.vDepthTex = float4(fDepth, fDepth, fDepth, 1.f);
	return vOut;
}
