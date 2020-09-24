
Texture2D    g_texture : register(t0);
SamplerState DiffuseSampler  : register(s0);
cbuffer cbPerObject : register(b0)
{
	float4x4 matWorld;
	float4x4 matView;
	float4x4 matProj;
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
	return g_texture.Sample(DiffuseSampler, In.vTexUV*30.f);
}

