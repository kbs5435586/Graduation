#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
	float3  vNormal		: NORMAL;
};
struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float4	vColor		: COLOR;
	float4  vNormal		: NORMAL;
	float4	vWorldPos		: TEXCOORD1;
	float4	vReflectionPosition : TEXCOORD0;
};
struct PS_OUT
{
	float4	vDiffuseTex			: SV_TARGET0;
	float4	vNormalTex			: SV_TARGET1;
	float4	vShadeTex			: SV_TARGET2;
	float4	vSpacularTex		: SV_TARGET3;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	//vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vPosition = mul(vOut.vPosition, matView);
	vOut.vPosition = mul(vOut.vPosition, matProj);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);

	vOut.vColor = vIn.vColor;
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWorld));

	matrix matReflectionProjWorld;

	matReflectionProjWorld = mul(matReflect, matProj);
	matReflectionProjWorld = mul(matWorld, matReflectionProjWorld);

	vOut.vReflectionPosition = mul(float4(vIn.vPosition, 1.f), matReflectionProjWorld);
	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT vOut = (PS_OUT)0;
	LIGHT tCol = (LIGHT)0.f;
	for (int i = 0; i < iNumLight; ++i)
	{
		LIGHT tCurCol = Calculate_Light(i, vIn.vNormal, vIn.vWorldPos);
		tCol.vDiffuse += tCurCol.vDiffuse;
		tCol.vAmbient += tCurCol.vAmbient;
		tCol.vSpecular += tCurCol.vSpecular;
	}

	float4 vNormal = float4((vIn.vNormal.xyz * 2.f) - 1.f, 0.f);

	vOut.vDiffuseTex = vIn.vColor;
	vOut.vNormalTex  = vNormal;
	vOut.vShadeTex	 = Calculate_Shade(vNormal);
	vOut.vSpacularTex = tCol.vSpecular;

	return vOut;
}

