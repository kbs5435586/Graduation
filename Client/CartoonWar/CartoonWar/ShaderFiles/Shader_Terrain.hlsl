#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float3 vViewPos		: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD;

};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal,0.f), matWV).xyz);
	vOut.vTexUV		= vIn.vTexUV;

	return vOut;
}

float4 PS_Main(VS_OUT vIn) : SV_Target
{
	float4 vOutColor = g_texture0.Sample(Sampler0, vIn.vTexUV);

	float3 vInNormal = vIn.vNormal;
	LIGHT  tLight = (LIGHT)0.f;

	for (int i = 0; i < iNumLight; ++i)
	{
		LIGHT tCurLight = Calculate_Light(i, vInNormal, vIn.vViewPos);
		tLight.vDiffuse += tCurLight.vDiffuse;
		tLight.vSpecular += tCurLight.vSpecular;
		tLight.vAmbient += tCurLight.vAmbient;
	}

	vOutColor.xyz = (tLight.vDiffuse.xyz*vOutColor.xyz) + (tLight.vSpecular.xyz) + (tLight.vAmbient.xyz*vOutColor.xyz);
	vOutColor.w = 1.f;
	return vOutColor;

}

