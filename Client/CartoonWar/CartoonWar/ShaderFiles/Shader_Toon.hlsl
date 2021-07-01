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
	float4	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float2	vTexUV		: TEXCOORD0;
	float4  vWorldPos	: TEXCOORD1;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0.f;

	if (g_int_0 == 1)
		Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal, vIn.vWeight, vIn.vIndices, 0);

	
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
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

	LIGHT tCurCol = Calculate_Light(0, vIn.vNormal.xyz, vIn.vWorldPos.xyz);

	float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4	vPosition = mul(vIn.vWorldPos, matViewInv);
	float4	vNormal = mul(vIn.vNormal, matViewInv);
	float4	vShade;
	
	float4	vLightDir = -tLight[0].vLightDir;
	float fDot = max(0, dot(vNormal, vLightDir));
	fDot = (ceil(fDot * 5.f) / 5.f);
	float4	vMtrlDif = float4(fDot, fDot, fDot,1.f);
	float4	vMtrlAmb = float4(0.7f, 0.7f, 0.7f,1.f);
	float3	fRimColor = float3(-2.f, -2.f, -2.f);
	float4 vView = normalize(vCamPos - vPosition);
	float	fRim = saturate(dot(vNormal, vView));
	if (fRim > 0.3f)
		fRim = 1.f;
	else
		fRim = -1.f;
	float	fRimPower = 2.f;
	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);

	vShade = (vMtrlDif + vMtrlAmb + vMtrlEmiv);
	



	//vMtrlDif *= tLight[0].tColor.vDiffuse;
	vOut.vDiffuseTex = saturate(vDiffuse * (vMtrlDif+ vMtrlAmb) + vMtrlEmiv);
	vOut.vNormalTex = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;

	return vOut;
}

