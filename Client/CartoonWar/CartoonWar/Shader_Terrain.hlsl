#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition		: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition		: SV_POSITION;
	float3 vViewPos			: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;

};

struct PS_OUT
{
	float4 vTarget0			: SV_TARGET0;
	float4 vTarget1			: SV_TARGET1;
	float4 vTarget2			: SV_TARGET2;
	float4 vTarget3			: SV_TARGET3;
	float4 vTarget4			: SV_TARGET4;
	float4 vTarget5			: SV_TARGET5;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut =(VS_OUT)0;

	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal,0.f), matWV).xyz);
	vOut.vTexUV		= vIn.vTexUV;



	return vOut;
}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	LIGHT tCol = (LIGHT)0.f;

	float3 vInNormal = vIn.vNormal;
	float3 vNorm;
	for (int i = 0; i < iNumLight; ++i)
	{
		LIGHT tCurCol = Calculate_Light(i, vInNormal, vIn.vViewPos);
		tCol.vDiffuse += tCurCol.vDiffuse;
		tCol.vAmbient += tCurCol.vAmbient;
		tCol.vSpecular += tCurCol.vSpecular;
		vNorm += normalize(mul(tLight[i].vLightDir, matView).xyz);
	}

	//LIGHT  tLight_ = Calculate_Light(0, vInNormal, vIn.vViewPos);

	float4 vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float3 vOutNormal = (vIn.vNormal.xyz-0.5f) * 2.f;


	vOut.vTarget0 = vOutColor;
	vOut.vTarget1.xyz = vOutNormal;
	vOut.vTarget2.xyz = vIn.vViewPos;
	vOut.vTarget3.xyz = saturate(dot(-vNorm, vInNormal));
	vOut.vTarget4.xyz = tCol.vSpecular;




	vOut.vTarget5.xyz = float3(iNumLight, iNumLight, iNumLight);
	return vOut;
}

