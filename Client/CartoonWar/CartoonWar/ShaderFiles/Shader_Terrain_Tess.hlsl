#include "Value.hlsl"
#include "Function.hlsl"


struct PatchTess
{
	float	fInsideTess		: SV_InsideTessFactor;
	float	arrEdgeTess[3]	: SV_TessFactor;
};

struct VS_IN
{
	float3 vPosition		: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD;
	float3 vBinormal		: BINORMAL;
	float3 vTanget			: TANGENT;
};

struct VS_OUT
{
	float3 vPosition		: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD;
	float3 vBinormal		: BINORMAL;
	float3 vTanget			: TANGENT;
};

struct HS_OUT
{
	float3 vPosition		: POSITION;
	float3 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD;
	float3 vBinormal		: BINORMAL;
	float3 vTanget			: TANGENT;
};


struct DS_OUT
{
	float4	vPosition			: SV_POSITION;
	float4	vNormal				: NORMAL;
	float2	vTexUV				: TEXCOORD0;
	float4	vWorldPos			: TEXCOORD1;
	float4	vBinormal			: BINORMAL;
	float4	vTanget				: TANGENT;
	float4	vProjPos			: TEXCOORD2;
};



struct PS_OUT
{
	float4	vDiffuseTex			: SV_TARGET0;
	float4	vNormalTex			: SV_TARGET1;
	float4	vPositionTex		: SV_TARGET2;
	float4	vDepthTex			: SV_TARGET3;
};


PatchTess PatchConstFunc_terrain(InputPatch<VS_OUT, 3> vIn, int iPatchID : SV_PrimitiveID)
{

	PatchTess output = { 0.f, { 0.f, 0.f, 0.f } };

	float3 vCenterPos = (vIn[0].vPosition + vIn[1].vPosition) / 2.f;
	float3 vSidePos = float3(vCenterPos.x - (vCenterPos.x - vIn[0].vPosition.x), vCenterPos.y, vCenterPos.z);
	float3 vUpDownPos = float3(vCenterPos.x, vCenterPos.y, vCenterPos.z - (vCenterPos.z - vIn[1].vPosition.z));

	vCenterPos = mul(float4(vCenterPos, 1.f), matWorld).xyz;
	vSidePos = mul(float4(vSidePos, 1.f), matWorld).xyz;
	vUpDownPos = mul(float4(vUpDownPos, 1.f), matWorld).xyz;

	float fCenter = CalTessLevel(vCamPos.xyz, vCenterPos, 10, 40, 4.f);
	float fSide = CalTessLevel(vCamPos.xyz, vSidePos, 10, 40, 4.f);
	float fUnDown = CalTessLevel(vCamPos.xyz, vUpDownPos, 10, 40, 4.f);

	output.arrEdgeTess[0] = fUnDown; // ¹Ø, À­
	output.arrEdgeTess[1] = fSide; // ÁÂ, ¿ì      
	output.arrEdgeTess[2] = fCenter; // ºøº¯
	output.fInsideTess = fCenter;

	return output;
}



VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut = vIn;
	return vOut;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstFunc_terrain")]
[maxtessfactor(64.f)]
[outputtopology("triangle_cw")]
HS_OUT HS_Main(InputPatch<VS_OUT, 3> vIn, int iVtxIdx : SV_OutputControlPointID, int iPatchID : SV_PrimitiveID)
{
	HS_OUT	vOut;
	vOut.vPosition = vIn[iVtxIdx].vPosition;
	vOut.vTexUV = vIn[iVtxIdx].vTexUV;
	vOut.vNormal = vIn[iVtxIdx].vNormal;
	vOut.vBinormal = vIn[iVtxIdx].vBinormal;
	vOut.vTanget = vIn[iVtxIdx].vTanget;
	return	vOut;
}


[domain("tri")]
DS_OUT DS_Main(const OutputPatch<HS_OUT, 3> vIn, float3 vLocation : SV_DomainLocation, PatchTess patch)
{
	DS_OUT vOut;

	float3	vLocalPosition = vIn[0].vPosition * vLocation[0] + vIn[1].vPosition * vLocation[1] + vIn[2].vPosition * vLocation[2];
	float2	vTexUV = vIn[0].vTexUV * vLocation[0] + vIn[1].vTexUV * vLocation[1] + vIn[2].vTexUV * vLocation[2];
	float3	vNormal = vIn[0].vNormal * vLocation[0] + vIn[1].vNormal * vLocation[1] + vIn[2].vNormal * vLocation[2];
	float3	vBinormal = vIn[0].vBinormal * vLocation[0] + vIn[1].vBinormal * vLocation[1] + vIn[2].vBinormal * vLocation[2];
	float3	vTanget = vIn[0].vTanget * vLocation[0] + vIn[1].vTanget * vLocation[1] + vIn[2].vTanget * vLocation[2];

	vOut.vPosition = mul(float4(vLocalPosition, 1.f), matWVP);
	vOut.vProjPos = mul(float4(vLocalPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vLocalPosition, 1.f), matWV);
	vOut.vNormal = mul(float4(vNormal, 0.f), matWV);
	vOut.vBinormal = mul(float4(vBinormal, 0.f), matWV);
	vOut.vTanget = mul(float4(vTanget, 0.f), matWV);

	vOut.vTexUV = vTexUV;
	return vOut;
}


PS_OUT	PS_Main(DS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4	vPosition = mul(vIn.vWorldPos, matViewInv);
	float4	vNormal = mul(vIn.vNormal, matViewInv);

	float4	vDiffuse;
	float3	vTSNormal;
	float4 vViewNormal = vIn.vNormal;
	float4	vFillterTex = g_texture8.Sample(Sampler0, vIn.vTexUV);
	if (vFillterTex.r == 1.f)
	{
		vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV * 30.f);
		vTSNormal = g_texture1.Sample(Sampler0, vIn.vTexUV * 30.f).xyz; \

			vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
		float3x3 matTBN = { vIn.vTanget.xyz, vIn.vBinormal.xyz, vIn.vNormal.xyz };
		vViewNormal.xyz = normalize(mul(vTSNormal, matTBN));
	}
	else if (vFillterTex.g == 0.f)
	{
		vDiffuse = g_texture2.Sample(Sampler0, vIn.vTexUV * 30.f);
		vTSNormal = g_texture3.Sample(Sampler0, vIn.vTexUV * 30.f).xyz;

		vTSNormal.xyz = (vTSNormal.xyz - 0.5f) * 2.f;
		float3x3 matTBN = { vIn.vTanget.xyz, vIn.vBinormal.xyz, vIn.vNormal.xyz };
		vViewNormal.xyz = normalize(mul(vTSNormal, matTBN));
	}


	float4	vBrushTex = (float4)0;
	if (vBrushPos.x - fBrushRange < vPosition.x && vPosition.x <= vBrushPos.x + fBrushRange &&
		vBrushPos.z - fBrushRange < vPosition.z && vPosition.z <= vBrushPos.z + fBrushRange)
	{
		float2		vTexUV;

		vTexUV.x = (vPosition.x - (vBrushPos.x - fBrushRange)) / (fBrushRange * 2.f);
		vTexUV.y = ((vBrushPos.z + fBrushRange) - vPosition.z) / (fBrushRange * 2.f);

		vBrushTex = g_texture9.Sample(Sampler0, vTexUV);
	}



	float4	vLightDir = -normalize(tLight[0].vLightDir);
	float	fDot = saturate(dot(vNormal, vLightDir));
	fDot = (ceil(fDot * 3.f) / 3.f);
	float4	vMtrlDif = float4(fDot, fDot, fDot, 0.f);
	float4	vMtrlAmb = float4(0.3f, 0.3f, 0.3f, 0.f);
	float3	fRimColor = float3(-2.f, -2.f, -2.f);
	float4	vView = normalize(vCamPos - vPosition);
	float	fRim = saturate(dot(vNormal, vView));
	float	fRimPower = 30.f;
	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower) * fRimColor, 1.f);



	float	fDepth = vIn.vWorldPos.z / 300.f;



	if (g_int_2)
	{
		vOut.vDiffuseTex = vDiffuse;
	}
	else
	{

		//vOut.vDiffuseTex = vDiffuse + vBrushTex;
		vOut.vDiffuseTex = (vDiffuse + vMtrlEmiv) + vBrushTex;
	}

	vOut.vNormalTex = vViewNormal;
	vOut.vPositionTex = vIn.vWorldPos;
	vOut.vDepthTex = float4(fDepth, fDepth, fDepth, 1.f);
	return vOut;
}
