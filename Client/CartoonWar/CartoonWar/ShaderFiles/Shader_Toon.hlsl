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
	float4	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float2	vTexUV		: TEXCOORD0;
	float4  vWorldPos	: TEXCOORD1;
	float4	vProjPos	: TEXCOORD2;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
	float4 vDepthTex			: SV_TARGET3;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0.f;

	if (g_int_0 == 1)
		Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal, vIn.vWeight, vIn.vIndices, 0);

	
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vProjPos = vOut.vPosition;
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), matWV).xyz);
	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWV).xyz);

	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut;

	float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vPosition = mul(vIn.vWorldPos, matViewInv);
	float4	vNormal = mul(vIn.vNormal, matViewInv);

	float4	vLightDir = -normalize(tLight[0].vLightDir);
	float	fDot = saturate(dot(vNormal, vLightDir));
	fDot = (ceil(fDot * 3.f) / 3.f);
	float4	vMtrlDif = float4(fDot, fDot, fDot,0.f);
	float4	vMtrlAmb = float4(0.3f, 0.3f, 0.3f,0.f);
	float3	fRimColor = float3(-2.f, -2.f, -2.f);
	float4	vView = normalize(vCamPos - vPosition);
	float	fRim = saturate(dot(vNormal, vView));

	float	fDepth = vIn.vWorldPos.z/300.f;


	if (fRim > 0.3f)
		fRim = 1.f;
	else
		fRim = -1.f;
	float	fRimPower = 2.f;
	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);

	if (g_int_2)
	{
		vOut.vDiffuseTex = vDiffuse;
	}
	else
	{
		vOut.vDiffuseTex = vDiffuse * (vMtrlDif+ vMtrlAmb + vMtrlEmiv);
	}

	vOut.vNormalTex = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	vOut.vDepthTex = float4(fDepth, fDepth, fDepth,1.f);
	return vOut;
}

