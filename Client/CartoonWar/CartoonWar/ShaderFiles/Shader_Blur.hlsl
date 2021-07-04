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
	float4	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float2	vTexUV		: TEXCOORD0;
	float4  vWorldPos	: TEXCOORD1;
	float4	vDir		: TEXCOORD2;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;
	if (g_int_0 == 1)
		Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal, vIn.vWeight, vIn.vIndices, 0);

	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), matWV).xyz);
	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWV).xyz);
	vOut.vTexUV = vIn.vTexUV;

	float4 vNewPos = vOut.vPosition;
	float4 vOldPos = mul(float4(vIn.vPosition, 1.f), g_mat_0);
	vOldPos = mul(vOldPos, g_mat_1);
	vOldPos = mul(vOldPos, matProj);

	float3	vDir = vNewPos.xyz - vOldPos.xyz;

	float a = dot(normalize(vDir), normalize(vOut.vNormal.xyz));
	if (a < 0.f)
		vOut.vPosition = vOldPos;
	else
		vOut.vPosition = vNewPos;

	float2 vVelocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
	vOut.vDir.xy = vVelocity * 0.5f;
	vOut.vDir.y *= -1.f;

	vOut.vDir.z = vOut.vPosition.z;
	vOut.vDir.w = vOut.vPosition.w;

	return vOut;
}
float4	PS_Main(VS_OUT vIn) :SV_TARGET
{
	float4 vOut;
	
	vOut.xy = vIn.vDir.xy;
	vOut.z = 1.f;
	vOut.w = vIn.vDir.z / vIn.vDir.w;

	return vOut;
}

