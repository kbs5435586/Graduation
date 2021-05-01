#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4	vDiffuse	: SV_TARGET0;
	float4	vSpecular	: SV_TARGET1;
	
};


VS_OUT	VS_DirLight(VS_IN vIn)
{
	VS_OUT	vOut;

	//vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vPosition = float4(vIn.vPosition ,1.f);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_DirLight(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vWorldPos = g_texture1.Sample(Sampler0, vIn.vTexUV);
	
	float4 vViewPos = mul(vWorldPos, matView);

	if (vViewPos.z <= 1.f)
		clip(-1);

	float4 vNormal = g_texture0.Sample(Sampler0, vIn.vTexUV);
	//LIGHT tCurCol = Calculate_Light(g_int_0, vNormal, vWorldPos);

	AD_Light tCurCol = Calculate_Light_Upgrade_V2(g_int_0, vNormal, vWorldPos);


	//tLight[iLightIdx].tColor.vDiffuse;
	float4 temp = float4(iNumLight, iNumLight, iNumLight, iNumLight);
	vOut.vDiffuse = tLight[0].tColor.vDiffuse;;
	//vOut.vDiffuse = temp;
	vOut.vSpecular = tCurCol.vSpecular;


	return vOut;
}



VS_OUT	VS_PointLight(VS_IN vIn)
{
	VS_OUT	vOut;

	//vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vPosition = float4(vIn.vPosition * 2.f, 1.f);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


PS_OUT	PS_PointLight(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vWorldPos = g_texture0.Sample(Sampler0, vIn.vTexUV);
	return vOut;
}



VS_OUT	VS_MergeLight(VS_IN vIn)
{
	VS_OUT	vOut;


	vOut.vPosition = float4(vIn.vPosition * 2.f, 1.f);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}

float4	PS_MergeLight(VS_OUT vIn) : SV_TARGET
{
	float4	vLightPow = g_texture1.Sample(Sampler0, vIn.vTexUV);
	if (vLightPow.x == 0.f && vLightPow.y == 0.f && vLightPow.z == 0.f)
	{
		clip(-1);
	}
	float4 vColor = g_texture0.Sample(Sampler0, vIn.vTexUV);
	float4 vSpec = g_texture2.Sample(Sampler0, vIn.vTexUV);


	return (vColor * vLightPow) + vSpec;
}