
Texture2D    g_texture : register(t0);
Texture2D    g_texture1 : register(t1);
SamplerState DiffuseSampler  : register(s0);
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
	float2 vTexUV : TEXCOORD;
};

struct VertexOut
{
	float4 vPosition  : SV_POSITION;
	float2 vTexUV : TEXCOORD;
};

VertexOut VS_Main(VertexIn In)
{
	VertexOut Out;
	Out.vPosition = mul(mul(mul(float4(In.vPosition, 1.0f), matWorld), matView), matProj);
	Out.vTexUV = In.vTexUV;

	return Out;
}

float4 PS_Main(VertexOut In) : SV_Target
{
	return g_texture.Sample(DiffuseSampler, In.vTexUV);
	//return float4(1.f,0.f,0.f,1.f);
}

