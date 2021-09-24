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
	float4	vProjPos	: TEXCOORD2;
};

struct PS_OUT
{
	float4 vOutLine			: SV_TARGET0;
};

float mask[9] =
{ -1, -1, -1,
  -1,  8, -1,
  -1, -1, -1 }; // Laplacian Filter

float coord[3] = { -1, 0, +1 };
float divider = 1;
float MAP_CX = 512;
float MAP_CY = 512;


VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut = (VS_OUT)0.f;

	if (g_int_0 == 1)
		Skinning(vIn.vPosition, vIn.vTangent, vIn.vBinormal, vIn.vNormal, vIn.vWeight, vIn.vIndices, 0);


	vOut.vPosition = mul(float4(vIn.vPosition + vIn.vNormal * g_float_0, 1.f), matWVP);
	vOut.vProjPos = vOut.vPosition;
	vOut.vWorldPos = mul(float4(vIn.vPosition, 1.f), matWV);
	vOut.vNormal = normalize(mul(float4(vIn.vNormal, 0.f), matWV));
	vOut.vTangent = normalize(mul(float4(vIn.vTangent, 0.f), matWV).xyz);
	vOut.vBinormal = normalize(mul(float4(vIn.vBinormal, 0.f), matWV).xyz);

	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}
float4 PS_Main(VS_OUT vIn) :SV_TARGET0
{
	float4	vOut;


	vOut = float4(0.f, 0.f, 0.f, 1.f);
	return vOut;
}
