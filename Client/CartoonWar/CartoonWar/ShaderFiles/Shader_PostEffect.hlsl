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

float4	PS_Main(VS_OUT vIn) : SV_TARGET0
{
	float2 vScreenUV = float2(vIn.vPosition.x / 1920, vIn.vPosition.y / 1080);
	
		
	float2 vDir = normalize(float2(0.5f, 0.5f) - vScreenUV);
	float fDist = distance(float2(0.5f, 0.5f), vScreenUV);

	// 왜곡 강도에 영향을 주는 중심으로부터 떨어진 비율( 중심에 가까울 수록 0에 가깝다.)
	float fRatio = (fDist / 0.5f);
	fRatio = fRatio * (0.2f + (fRatio) * 0.4f);

	//                                                 속도     진폭                                         
	vScreenUV += vDir * sin(abs((fRatio - g_fAccTime * 0.06f) * 40.f)) * 0.03f;

	return g_texture0.Sample(Sampler0, vScreenUV);
}

