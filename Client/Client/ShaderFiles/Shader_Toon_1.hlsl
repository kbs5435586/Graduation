Texture2D    g_texture : register(t0);
SamplerState DiffuseSampler  : register(s4);



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
};

struct VS_OUT
{
	float4	vPos : SV_POSITION;
	float4	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	float4	vWorldPos : TEXCOORD1;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPos = mul(mul(mul(float4(vIn.vPos, 1.0f), matWorld), matView), matProj);

	vOut.vNormal = normalize(mul(vector(vIn.vNormal, 0.f), matWorld));
	vOut.vTexUV = vIn.vTexUV;
	vOut.vWorldPos = mul(vector(vIn.vPos, 1.f), matWorld);
	vOut.vNormal = normalize(mul(vector(vIn.vNormal, 0.f), matWorld));
	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	float4	vOutColor = g_texture.Sample(DiffuseSampler, vIn.vTexUV);

	
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

	return vOutColor;

}

