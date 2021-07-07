#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition		: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD;
	float3 vBinormal		: BINORMAL;
	float3 vTanget			: TANGENT;
};

struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float4	vNormal				: NORMAL;
	float2	vTexUV				: TEXCOORD0;
	float4	vWorldPos			: TEXCOORD1;
	float4	vBinormal			: BINORMAL;
	float4	vTanget				: TANGENT;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));

	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWV));
	vOut.vTanget = normalize(mul(float4(vIn.vTanget, 0.f), matWV));
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}



//PS_OUT PS_Main(VS_OUT vIn)
//{
//	PS_OUT vOut = (PS_OUT)0;
//
//	AD_Light	tLight_Default = Calculate_Light_Upgrade(0, vIn.vNormal, vIn.vWorldPos);
//	AD_Light	tLight_Point_Red = Calculate_Light_Upgrade(1, vIn.vNormal, vIn.vWorldPos);
//
//	float4	vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
//	float4	vFogColor = float4(0.5f, 0.5f, 0.5f, 1.f);
//
//	vOut.vDiffuseTex = vOutColor;
//	vOut.vNormalTex = vIn.vNormal;
//	vOut.vPositionTex = vIn.vWorldPos;
//	vOut.vShadeTex = tLight_Point_Red.vShade + tLight_Default.vShade;
//	vOut.vPointLightTex = tLight_Point_Red.vDiffuse;
//
//
//	return vOut;
//}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV* 30.f);
	float4	vPosition = mul(vIn.vWorldPos, matViewInv);
	float4	vNormal = mul(vIn.vNormal, matViewInv);



	float4	vLightDir = -normalize(tLight[0].vLightDir);
	float	fDot = saturate(dot(vNormal, vLightDir));
	fDot = (ceil(fDot * 3.f) / 3.f);
	float4	vMtrlDif = float4(fDot, fDot, fDot, 0.f);
	float4	vMtrlAmb = float4(0.3f, 0.3f, 0.3f, 0.f);

	float3 vTSNormal = g_texture1.Sample(Sampler0, vIn.vTexUV * 30.f).xyz;
	vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
		
	float3x3 matTBN = { vIn.vTanget.xyz, vIn.vBinormal.xyz, vIn.vNormal.xyz };
	vNormal = float4(normalize(mul(vTSNormal, matTBN)), 0.f);
		
	

	float3	fRimColor = float3(-2.f, -2.f, -2.f);
	float4	vView = normalize(vCamPos - vPosition);
	float	fRim = saturate(dot(vNormal, vView));
	float	fRimPower = 50.f;
	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);

	if (g_int_2)
	{
		vOut.vDiffuseTex = vDiffuse;
	}
	else
	{
		
		vOut.vDiffuseTex = vDiffuse;
		//vOut.vDiffuseTex = (vDiffuse +  vMtrlEmiv);
	}

	vOut.vNormalTex = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	return vOut;
}
