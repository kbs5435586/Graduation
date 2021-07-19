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
	float4 vPosition : SV_Position;
	float2 vTexUV : TEXCOORD;
};
struct PS_OUT
{
	float4	vReflectionTex : SV_TARGET0;
	float4	vRefractionTex : SV_TARGET1;
};
VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;
	if (g_int_0)
	{
		Skinning(vIn.vPosition, vIn.vWeight, vIn.vIndices, 0);
	}
	vOut.vPosition = mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}
PS_OUT	PS_Main(VS_OUT vIn) 
{
	PS_OUT	vOut = (PS_OUT)0;
	float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);

	vOut.vReflectionTex = vDiffuse;
	vOut.vRefractionTex = vDiffuse;

	return vOut;
}

