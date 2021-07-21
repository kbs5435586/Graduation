#include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition	: POSITION;
	float4	vColor		: COLOR;
	float3	vNormal		: NORMAL;
};
struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float4	vColor		: COLOR;
	float4	vNormal		: NORMAL;
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
	VS_OUT	vOut;

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));
	vOut.vColor = vIn.vColor;
	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4 vColor = (float4)0;

	if (g_int_0 == 0)
	{
		vColor = float4(1.f, 1.f, 1.f,1.f);
	}
	else if(g_int_0 ==1)
	{
		vColor = float4(1.f, 0.f, 0.f, 1.f);
	}

	vOut.vDiffuseTex = vColor;
	vOut.vNormalTex = vIn.vNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	return vOut;
}

