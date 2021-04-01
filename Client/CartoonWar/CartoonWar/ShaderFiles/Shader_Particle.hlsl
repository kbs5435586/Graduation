#include "Value.hlsl"
#include "Function.hlsl"



StructuredBuffer<tParticle> tData : register(t10);
//RWStructuredBuffer<tParticle> tRWData : register(u0);
//RWStructuredBuffer<tParticleShared> tRWSharedData : register(u1);

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vUV : TEXCOORD;

	uint iID : SV_InstanceID;
};
struct VS_OUT
{
	float4 vWorldPos : POSITION;
	float2 vUV : TEXCOORD;
	float iInstID : FOG;
};
struct PS_OUT
{
	float4 vTarget : SV_TARGET0;
};

struct GS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;
	uint iInstID : SV_InstanceID;
};

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;


	float3 vWorldPos = mul(float4(vIn.vPosition, 1.f), matWorld).xyz;
	vWorldPos += tData[vIn.iID].vWorldPos;

	vOut.vWorldPos = float4(vWorldPos, 1.f);
	vOut.vUV = vIn.vUV;
	vOut.iInstID = vIn.iID;

	return vOut;
}


[maxvertexcount(6)]
void GS_Main(point VS_OUT _in[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output[4] =
	{
		(GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f, (GS_OUT)0.f
	};
	uint iInstID = (uint) _in[0].iInstID;

	if (0 == tData[iInstID].iAlive)
		return;

	float fRatio = tData[iInstID].m_fCurTime / tData[iInstID].m_fLifeTime;
	float fCurScale = ((g_float_1 - g_float_0) * fRatio + g_float_0) / 2.f;

	output[0].vPosition = _in[0].vWorldPos + float4(-fCurScale, fCurScale, 0.f, 0.f);
	output[1].vPosition = _in[0].vWorldPos + float4(fCurScale, fCurScale, 0.f, 0.f);
	output[2].vPosition = _in[0].vWorldPos + float4(fCurScale, -fCurScale, 0.f, 0.f);
	output[3].vPosition = _in[0].vWorldPos + float4(-fCurScale, -fCurScale, 0.f, 0.f);

	float4x4	matVP = matView * matProj;
	output[0].vPosition = mul(output[0].vPosition, matVP);
	output[1].vPosition = mul(output[1].vPosition, matVP);
	output[2].vPosition = mul(output[2].vPosition, matVP);
	output[3].vPosition = mul(output[3].vPosition, matVP);

	output[0].vUV = float2(0.f, 0.f);
	output[1].vUV = float2(1.f, 0.f);
	output[2].vUV = float2(1.f, 1.f);
	output[3].vUV = float2(0.f, 1.f);

	output[0].iInstID = iInstID;
	output[1].iInstID = iInstID;
	output[2].iInstID = iInstID;
	output[3].iInstID = iInstID;


	OutputStream.Append(output[0]);
	OutputStream.Append(output[1]);
	OutputStream.Append(output[2]);
	OutputStream.RestartStrip();

	OutputStream.Append(output[0]);
	OutputStream.Append(output[2]);
	OutputStream.Append(output[3]);
	OutputStream.RestartStrip();
}


float4 PS_Main(GS_OUT _in)	: SV_TARGET
{


	float fRatio = tData[_in.iInstID].m_fCurTime / tData[_in.iInstID].m_fLifeTime;
	float4 vCurColor = (g_vec4_1 - g_vec4_0) * fRatio + g_vec4_0;

	



	return vCurColor *= g_texture0.Sample(Sampler0, _in.vUV);;
}

//
//PS_OUT	PS_Main(GS_OUT _in) 
//{
//	PS_OUT vOut = (PS_OUT)0;
//
//
//	float fRatio = tData[_in.iInstID].m_fCurTime / tData[_in.iInstID].m_fLifeTime;
//	float4 vCurColor = (g_vec4_1 - g_vec4_0) * fRatio + g_vec4_0;
//	 vCurColor *= g_texture0.Sample(Sampler0, _in.vUV);
//
//	vOut.vTarget = vCurColor;
//
//	return vOut;
//}
//
