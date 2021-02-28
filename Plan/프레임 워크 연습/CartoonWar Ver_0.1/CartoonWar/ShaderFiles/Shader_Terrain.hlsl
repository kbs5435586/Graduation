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
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vWorldPos		: TEXCOORD1;

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
	vOut.vWorldPos  = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal,0.f), matWorld));
	vOut.vTexUV		= vIn.vTexUV;



	return vOut;
}

PS_OUT PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;
	LIGHT tCol = (LIGHT)0.f;

	float4 vNorm	 = float4(0.f,0.f,0.f,0.f);


	for (int i = 0; i < iNumLight; ++i)
	{
		LIGHT tCurCol	 = Calculate_Light(i, vIn.vNormal, vIn.vWorldPos);
		tCol.vDiffuse	+= tCurCol.vDiffuse;
		tCol.vAmbient	+= tCurCol.vAmbient;
		tCol.vSpecular	+= tCurCol.vSpecular;
		vNorm			+= normalize(mul(tLight[i].vLightDir, matWorld));
	}
	float4 vOutColor	 = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4 vOutNormal	 = float4((vIn.vNormal.xyz-0.5f) * 2.f,0.f);



	vOut.vTarget0		 = vOutColor;
	vOut.vTarget1		 = vOutNormal;
	vOut.vTarget2		 = tCol.vAmbient;
	vOut.vTarget3.xyz	 = saturate(dot(-vNorm, vIn.vNormal));
	vOut.vTarget4.xyz	 = tCol.vSpecular;

	vOut.vTarget5.xyz = float3(iNumLight, iNumLight, iNumLight);
	return vOut;
}

