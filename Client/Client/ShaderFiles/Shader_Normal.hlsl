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
	float4		vLightSpecular;
	float4		vLightAmbient;
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

	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	float4	OutColor;
	float4	vDiffuseColor = g_texture.Sample(DiffuseSampler, vIn.vTexUV);

	vector	vShade;
	vShade = max(dot(normalize(vLightDirection) * -1.f, normalize(vIn.vNormal)), 0.f);

	vector	vSpecular;
	vector	vReflect = reflect(normalize(vLightDirection), normalize(vIn.vNormal));
	vector	vLook = vIn.vWorldPos - vCamPos;

	vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), fPower);

	OutColor = ((vLightDiffuse * vDiffuseColor) +
		(vLightSpecular * vMaterialSpecular) * vSpecular) *
		saturate(vShade + (vLightAmbient * vMaterialAmbient));



	OutColor.a = 1.f;

	return OutColor;
}

