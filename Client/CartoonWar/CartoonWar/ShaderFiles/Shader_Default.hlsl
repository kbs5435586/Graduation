 #include "Value.hlsl"
#include "Function.hlsl"

struct VS_IN
{
	float3	vPosition			: POSITION;
	float4	vColor				: COLOR;
	float3  vNormal				: NORMAL;
};
struct VS_OUT
{
	float4	vPosition			: SV_POSITION;
	float4	vColor				: COLOR;
	float4  vNormal				: NORMAL;
	float4	vWorldPos			: TEXCOORD1;
	float4  vProjPos			: TEXCOORD2;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	//vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);

	// -0.5f, 0.5f, 0.f,1.f
	vOut.vColor = vIn.vColor;
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWorld));
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld);
	vOut.vProjPos = vOut.vPosition;

	return vOut;
}
float4	PS_Main(VS_OUT vIn)  : SV_TARGET
{
	float	fAccTime = g_float_0;
	float4	vOutColor;

	vOutColor = float4(1.f, fAccTime/5.f, 0.f, 1.f);

	return vOutColor;
}

