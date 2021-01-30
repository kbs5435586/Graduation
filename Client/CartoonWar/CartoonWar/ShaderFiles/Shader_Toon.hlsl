Texture2D		g_texture			: register(t0);
Texture2D		g_Normal_Texture	: register(t1);
Texture2D		g_texture2			: register(t2);
Texture2D		g_texture3			: register(t3);

SamplerState	DiffuseSampler		: register(s0);



cbuffer cbPerObject : register(b0)
{
	float4x4	matWorld;
	float4x4	matView;
	float4x4	matProj;

	float4		vCamPos;

	float4		vMaterialDiffuse;
	float4		vMaterialSpecular;
	float4		vMaterialAmbient;

	float4		vLightDiffuse;
	float4		vLightAmbient;
	float4		vLightSpecular;
	float4		vLightDirection;

	float		fPower;
};

struct VS_IN
{
	float3	vPos : POSITION;
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD;
	float3	vTangent : TANGENT;
	float3	vBinormal: BINORMAL;
};

struct VS_OUT
{
	float4	vPos : SV_POSITION;
	float4	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	float4	vWorldPos : TEXCOORD1;
	float4	vTangent : TANGENT;
	float4	vBinormal: BINORMAL;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPos = mul(mul(mul(float4(vIn.vPos, 1.0f), matWorld), matView), matProj);

	vOut.vNormal = normalize(mul(vector(vIn.vNormal, 0.f), matWorld));
	vOut.vTexUV = vIn.vTexUV;
	vOut.vWorldPos = mul(vector(vIn.vPos, 1.f), matWorld);
	vOut.vNormal = normalize(mul(vector(vIn.vNormal, 0.f), matWorld));
	vOut.vTangent = normalize(mul(vector(vIn.vTangent, 0.f), matWorld));
	vOut.vBinormal = normalize(mul(vector(vIn.vBinormal, 0.f), matWorld));
	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	/*float4	vOutColor = g_texture.Sample(DiffuseSampler, vIn.vTexUV) ;

	
	float fDot = max(0, dot(vIn.vNormal, vLightDirection));

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
	float4	vTexture = g_texture.Sample(DiffuseSampler, vIn.vTexUV);
	float4	vNormalMap = g_Normal_Texture.Sample(DiffuseSampler, vIn.vTexUV);
	float4	vOutColor;
	float3	vLightDir;
	float4	vDiffuseColor = float4(1.f,1.f,1.f,1.f);
	float	fLightIntendity = 0.f;

	vNormalMap = (vNormalMap * 2.f) - 1.f;

	float3 vBumpNormal = (vIn.vNormal.xyz + vNormalMap.x) * (vIn.vTangent.xyz + vNormalMap.y) * vIn.vBinormal.xyz;

	vBumpNormal = normalize(vBumpNormal);

	vLightDir = -vLightDirection.xyz;
	fLightIntendity = saturate(dot(vBumpNormal, vLightDir));


	vOutColor = saturate(vDiffuseColor * fLightIntendity);

	vOutColor = vOutColor * vTexture;

	return vOutColor;
}

