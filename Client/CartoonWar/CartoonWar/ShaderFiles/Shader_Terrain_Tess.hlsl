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
	float2 vTexUV			: TEXCOORD;
};

struct VS_OUT
{
	float3 vPosition		: POSITION;
	float2 vTexUV			: TEXCOORD;
};

struct HS_OUT
{
	float3 vPosition		: POSITION;
	float2 vTexUV			: TEXCOORD;
};


struct DS_OUT
{
	float4	vPosition			: SV_POSITION;
	float2	vTexUV				: TEXCOORD0;
	float4	vWorldPos			: TEXCOORD1;
	float4	vNormal				: NORMAL;
};

struct PS_OUT
{
	float4 vDiffuseTex			: SV_TARGET0;
	float4 vNormalTex			: SV_TARGET1;
	float4 vPositionTex			: SV_TARGET2;
	float4 vDepthTex			: SV_TARGET3;
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

	float fCenter = CalTessLevel(vCamPos.xyz, vCenterPos, 1000.f, 4000.f, 4.f);
	float fSide = CalTessLevel(vCamPos.xyz, vSidePos, 1000.f, 4000.f, 4.f);
	float fUnDown = CalTessLevel(vCamPos.xyz, vUpDownPos, 1000.f, 4000.f, 4.f);



	output.arrEdgeTess[0] = fUnDown; // 关, 拉
	output.arrEdgeTess[1] = fSide ; // 谅, 快      
	output.arrEdgeTess[2] = fCenter ; // 壶函
	output.fInsideTess = fCenter ;

	return output;
}

PatchTess PatchConstFunc(InputPatch<VS_OUT, 3> vIn, int iPatchID : SV_PrimitiveID)
{
    PatchTess output = { 0.f, { 0.f, 0.f, 0.f } };
    
  
    output.arrEdgeTess[0] = 10.f; // 关, 拉
    output.arrEdgeTess[1] = 10.f; // 谅, 快      
    output.arrEdgeTess[2] = 10.f; // 壶函
    output.fInsideTess = 10.f;
    
    return output;
}



VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut = vIn;
	return vOut;
}

[domain("tri")]
//[partitioning("fractional_odd")]
[partitioning("integer")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(64.f)]
[outputtopology("triangle_cw")]
HS_OUT HS_Main(InputPatch<VS_OUT, 3> vIn, int iVtxIdx : SV_OutputControlPointID, int iPatchID : SV_PrimitiveID)
{
	HS_OUT	vOut;
	vOut.vPosition = vIn[iVtxIdx].vPosition;
	vOut.vTexUV = vIn[iVtxIdx].vTexUV;
	return	vOut;
}


[domain("tri")]
DS_OUT DS_Main(const OutputPatch<HS_OUT, 3> vIn, float3 vLocation : SV_DomainLocation, PatchTess patch)
{
	DS_OUT vOut;
	
	float3	vLocalPosition = vIn[0].vPosition * vLocation[0] + vIn[1].vPosition * vLocation[1] + vIn[2].vPosition * vLocation[2];
	float2	vTexUV = vIn[0].vTexUV * vLocation[0] + vIn[1].vTexUV * vLocation[1] + vIn[2].vTexUV * vLocation[2];
	float2	vFullUV = float2(vTexUV.x / 100.f, vTexUV.y / 100.f);

	float	fHeight = cos(g_fAccTime* vTexUV.x)*2.f;
	//float	fHeight = cos(g_fAccTime * vTexUV.x ) * 0.8f; 
	vLocalPosition.z += fHeight;

	float3	vTemp = float3(0.f, 0.f, 1.f);
	vOut.vPosition = mul(float4(vLocalPosition, 1.f), matWVP);
	vOut.vWorldPos = mul(float4(vLocalPosition, 1.f), matWV);
	vOut.vTexUV = vTexUV;
	vOut.vNormal = mul(float4(vTemp, 0.f), matWV);
	
	return vOut;
}
//
//float4	PS_Main(DS_OUT vIn) :SV_TARGET0
//{
//	float2 vScreenUV = float2(vIn.vPosition.x / 1920, vIn.vPosition.y / 1080);
//	float2 vDir = normalize(float2(0.5f, 0.5f) - vIn.vTexUV);
//	float fDist = distance(float2(0.5f, 0.5f), vIn.vTexUV);
//
//	float fRatio = (fDist / 0.5f);
//	fRatio = fRatio * (0.2f + (fRatio) * 0.4f);
//
//	vScreenUV += vDir * sin(abs((fRatio - g_fAccTime * 0.06f) * 80.f)) * 0.03f;
//	vScreenUV.x += fFrameTime;
//	vIn.vTexUV.x += fFrameTime;
//	float4 vDiffuse = g_texture0.Sample(Sampler0, vScreenUV);
//	float4 vDiffuse_T = g_texture0.Sample(Sampler0, vIn.vTexUV);
//
//
//	return vDiffuse;
//}


PS_OUT	PS_Main(DS_OUT vIn)
{
	PS_OUT vOut = (PS_OUT)0;

	float4	vPosition = mul(vIn.vWorldPos, matViewInv);
	float4	vNormal = mul(vIn.vNormal, matViewInv);
	float4	vDiffuse = g_texture0.Sample(Sampler0, vIn.vTexUV);



	vOut.vDiffuseTex = vDiffuse;
	vOut.vPositionTex = vIn.vWorldPos;
	vOut.vNormalTex = vIn.vNormal;
	vOut.vDepthTex = float4(1.f, 1.f, 1.f, 1.f);
	return vOut;
}
