
Texture2D		g_texture : register(t0);
Texture2D		g_texture1 : register(t1);
Texture2D		g_texture2 : register(t2);
Texture2D		g_texture3 : register(t3);


SamplerState	DiffuseSampler  : register(s2);
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

struct VertexIn
{
	float3 vPosition  : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD;
};

struct VertexOut
{
	float4 vPosition  : SV_POSITION;
	float4 vNormal: NORMAL;
	float2 vTexUV : TEXCOORD;
};

VertexOut VS_Main(VertexIn In)
{
	VertexOut Out;

	Out.vPosition = mul(mul(mul(float4(In.vPosition, 1.0f), matWorld), matView), matProj);
	Out.vNormal = float4(In.vNormal,1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}

float4 PS_Main(VertexOut In) : SV_Target
{
	return g_texture.Sample(DiffuseSampler, In.vTexUV )+ g_texture1.Sample(DiffuseSampler, In.vTexUV);
}

