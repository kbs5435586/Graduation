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
	//vOut.vNormal = normalize(mul(vector(vIn.vNormal, 0.f), matWorld));
	vOut.vNormal = float4(vIn.vNormal, 1.f);
	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	float4	OutColor = g_texture.Sample(DiffuseSampler, vIn.vTexUV);

	float3	rimcolor = float3(-2.f, -2.f, -2.f);
	int		rimpower = 5.f;
	float	rim = saturate(dot(vIn.vNormal, vCamPos));
	if (rim > 0.3f)
		rim = 1.f;
	else
		rim = -1.f;
	float4	vMtrlDif = vLightDiffuse * vMaterialDiffuse;
	float4  vMtrlEmsv = float4(pow(1 - rim, rimpower) * rimcolor, 1.f);

	return float4(OutColor * (vMtrlDif+vMtrlEmsv));
}

