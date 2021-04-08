#include "Value.hlsl"
#include "Function.hlsl"

//XMFLOAT3		vPosition;
//XMFLOAT4		vColor;
//XMFLOAT2		vUV;
//XMFLOAT3		vNormal;
//XMFLOAT3		vTangent;
//XMFLOAT3		vBinormal;
//XMFLOAT4		vWeight;
//XMFLOAT4		vIndices;

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
	float4	vTemp		: TEXCOORD2;

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

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0.f;
	StructuredBuffer<float4x4> matTemp = g_arrFinalBoneMat;

	//Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal , vIn.vWeight, vIn.vIndices, 0);

	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos  = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal, 0.f), matWVP).xyz);
	vOut.vTangent	= normalize(mul(float4(vIn.vTangent, 0.f), matWVP).xyz);
	vOut.vBinormal	= normalize(mul(float4(vIn.vBinormal, 0.f), matWVP).xyz);
	vOut.vTemp		= vIn.vIndices;

	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut;



	float4 vTempNormal = float4(vIn.vNormal, 0.f);
	AD_Light	tLight_Default = Calculate_Light_Upgrade(0, vTempNormal, vIn.vWorldPos);
	AD_Light	tLight_Point = Calculate_Light_Upgrade(1, vTempNormal, vIn.vWorldPos);

	float4	vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV) ;
	float4	vOutColor_ = vIn.vTemp;

	float4	vLightDir = normalize(tLight[0].vLightDir);

	float fDot = max(0, dot(vTempNormal, vLightDir));

	fDot = (ceil(fDot * 3.f) / 3.f);

	float4	vMtrlDif = tLight[0].tColor.vDiffuse *  fDot;
	float4	vMtrlAmb = tLight[0].tColor.vAmbient * fDot;


	float3	fRimColor = float3(-2.f,-2.f,-2.f);
	float	fRim = saturate(dot(vTempNormal, vCamPos));
	float	fRimPower = 2.f;
	if (fRim > 0.3f)
		fRim = 1.f;
	else
		fRim = -1.f;

	vector	vReflect = reflect(normalize(vLightDir), normalize(vTempNormal));
	vector	vLook = vIn.vWorldPos - vCamPos;
	float4	vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 10.f);
	float4	vMtrlSpec = (tLight[0].tColor.vSpecular * tLight[0].tColor.vSpecular) * vSpecular;

	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower)*fRimColor,1.f);

	vOutColor = vOutColor*(vMtrlDif + vMtrlAmb+ vMtrlSpec +vMtrlEmiv);

	vOut.vDiffuseTex = vOutColor;
	vOut.vNormalTex = vOutColor;
	//vOut.vShadeTex = vIn.vTemp;
	//vOut.vSpecularTex = vIn.vTemp;
	vOut.vPointLightTex = vOutColor_;
	vOut.vPositionTex = vOutColor;
	//vOut.vNormalTex = vTempNormal;
	//vOut.vShadeTex = tLight_Point.vShade + tLight_Default.vShade;
	//vOut.vSpecularTex= tLight_Point.vSpecular + tLight_Default.vSpecular;
	//vOut.vPointLightTex= tLight_Point.vDiffuse;
	//vOut.vPositionTex= vIn.vWorldPos;

	return vOut;


	{
		// Normal Mapping
//float4	vTexture = g_texture.Sample(DiffuseSampler, vIn.vTexUV);
//float4	vNormalMap = g_Normal_Texture.Sample(DiffuseSampler, vIn.vTexUV);
//float4	vOutColor;
//float3	vLightDir;
//float4	vDiffuseColor = float4(1.f,1.f,1.f,1.f);
//float	fLightIntendity = 0.f;

//vNormalMap = (vNormalMap * 2.f) - 1.f;

//float3 vBumpNormal = (vIn.vNormal.xyz + vNormalMap.x) * (vIn.vTangent.xyz + vNormalMap.y) * vIn.vBinormal.xyz;

//vBumpNormal = normalize(vBumpNormal);

//vLightDir = -vLightDirection.xyz;
//fLightIntendity = saturate(dot(vBumpNormal, vLightDir));


//vOutColor = saturate(vDiffuseColor * fLightIntendity);

//vOutColor = vOutColor * vTexture;

//return vOutColor;

	}



	//Default
	//return g_texture0.Sample(Sampler0, vIn.vTexUV);
}

