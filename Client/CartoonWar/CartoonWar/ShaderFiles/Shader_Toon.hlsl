#include "Value.hlsl"
#include "Function.hlsl"
struct VS_IN
{
	float3	vPosition	: POSITION;
	float3	vNormal		: NORMAL;
	float2	vTexUV		: TEXCOORD;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float3	vNormal		: NORMAL;
	float3	vTangent	: TANGENT;
	float3	vBinormal	: BINORMAL;
	float3	vViewPos	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition	= mul(float4(vIn.vPosition, 1.f), matWVP);
	vOut.vViewPos	= mul(float4(vIn.vPosition, 1.f), matWV).xyz;
	vOut.vNormal	= normalize(mul(float4(vIn.vNormal, 0.f), matWV).xyz);
	vOut.vTangent	= normalize(mul(float4(vIn.vTangent, 0.f), matWV).xyz);
	vOut.vBinormal	= normalize(mul(float4(vIn.vBinormal, 0.f), matWV).xyz);


	vOut.vTexUV = vIn.vTexUV;
	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	//float4	vOutColor = g_texture.Sample(DiffuseSampler, vIn.vTexUV) ;


	/*float fDot = max(0, dot(vIn.vNormal, vLightDirection));

	fDot = (ceil(fDot * 3.f) / 3.f);

	float4	vMtrlDif = vLightDiffuse * vMaterialDiffuse * fDot;
	float4	vMtrlAmb = vLightAmbient * vMaterialAmbient * fDot;


	float3	fRimColor = float3(-2.f,-2.f,-2.f);
	float	fRim = saturate(dot(vIn.vNormal, vCamPos));
	float	fRimPower = 2.f;
	if (fRim > 0.3f)
		fRim = 1.f;
	else
		fRim = -1.f;

	vector	vReflect = reflect(normalize(vLightDirection), normalize(vIn.vNormal));
	vector	vLook = vIn.vWorldPos - vCamPos;
	float4	vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), fPower);
	float4	vMtrlSpec = (vLightSpecular * vLightSpecular) * vSpecular;

	float4	vMtrlEmiv = float4(pow(1.f - fRim, fRimPower)*fRimColor,1.f);

	vOutColor = vOutColor*(vMtrlDif + vMtrlAmb+ vMtrlSpec +vMtrlEmiv);

	return vOutColor;*/





	// Normal Mapping
	//float4	vTexture = g_texture.Sample(DiffuseSampler, vIn.vTexUV);
	//float4	vNormalMap = g_Normal_Texture.Sample(DiffuseSampler, vIn.vTexUV);
	//float4	vOutColor;
	//float3	vLightDir;
	//float4	vDiffuseColor = float4(1.f,1.f,1.f,1.f);
	//float	fLightIntendity = 0.f;

	//vNormalMap = (vNormalMap * 2.f) - 1.f;

	//float3 vBumpNormal = (vIn.vNormal.xyz + vNormalMap.x) * (vIn.vTangent.xyz + vNormalMap.y) * vIn.vBinormal.xyz;

	//vBumpNormal = normalize(vBumpNormal);

	//vLightDir = -vLightDirection.xyz;
	//fLightIntendity = saturate(dot(vBumpNormal, vLightDir));


	//vOutColor = saturate(vDiffuseColor * fLightIntendity);

	//vOutColor = vOutColor * vTexture;

	//return vOutColor;



	//Default
	return g_texture0.Sample(Sampler0, vIn.vTexUV);


}

