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
	float3	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float2	vTexUV		: TEXCOORD0;
	float4  vWorldPos	: TEXCOORD1;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET5;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
	float4 vShadeTex			: SV_TARGET3;
	//float4 vPointLightTex		: SV_TARGET4;
	float4 vSpecularTex			: SV_TARGET4;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0.f;

	if (g_int_0 == 1)
		Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal, vIn.vWeight, vIn.vIndices, 0);


	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), I_matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), I_matWorld);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), I_matWVP).xyz);
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), I_matWVP).xyz);
	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), I_matWVP).xyz);

	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut;
	AD_Light tCol = (AD_Light)0;

	float4	vNormal = float4(vIn.vNormal, 0.f);
	float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vSpecular = float4(0.f, 0.f, 0.f, 0.f);
	float4	vPointLight = float4(0.f, 0.f, 0.f, 0.f);
	//AD_Light   tLight_Point = Calculate_Light_Upgrade(1, vNormal, vIn.vWorldPos);

	for (int i = 0; i < iNumLight; ++i)
	{
		AD_Light tCurCol = Calculate_Light_Upgrade_V2(i, vNormal, vIn.vWorldPos);
		tCol.vDiffuse += tCurCol.vDiffuse;
		tCol.vSpecular += tCurCol.vSpecular;
		tCol.vAmbient += tCurCol.vAmbient;
		tCol.vShade += tCurCol.vShade;
	}

	//float4	vLightDir = normalize(tLight[0].vLightDir);

	float4	vLightWorldPos = tLight[0].vLightPos;
	float4	vLightDir = normalize(vIn.vWorldPos - vLightWorldPos);

	//float4	vLightDir = normalize(tLight[0].vLightDir);

	float fDot = max(0, dot(vNormal, vLightDir));


	float4 vView = normalize(vCamPos - vIn.vWorldPos);
	fDot = (ceil(fDot * 5.f) / 5.f);

	float4	vMtrlDif = tLight[0].tColor.vDiffuse * fDot;
	float4	vMtrlAmb = tLight[0].tColor.vAmbient;

	float3	fRimColor = float3(-2.f, -2.f, -2.f);

	float	fRim = saturate(dot(vView, vNormal));
	float	fRimPower = 8.f;


	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);

	float4 vShade = (vMtrlDif + vMtrlAmb + vMtrlEmiv);



	//vOut.vDiffuseTex = vDiffuse * vShade;
	vOut.vDiffuseTex = vDiffuse * vShade;
	//vOut.vNormalTex = vNormal;
	//vOut.vPositionTex = vIn.vWorldPos;
	//vOut.vShadeTex = vShade;
	//vOut.vSpecularTex = tCol.vShade;

	return vOut;
}
