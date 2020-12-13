
TextureCube    g_texture : register(t0);
SamplerState DiffuseSampler  : register(s2);
cbuffer cbPerObject : register(b0)
{
	float4x4 matWorld;
	float4x4 matView;
	float4x4 matProj;
};

struct VertexIn
{
	float3 vPosition  : POSITION;
	float3 vTexUV : TEXCOORD;
};

struct VertexOut
{
	float4 vPosition  : SV_POSITION;
	float3 vTexUV : TEXCOORD;
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
	//return texCUBE(DiffuseSampler, In.vTexUV);
	return g_texture.Sample(DiffuseSampler, In.vTexUV);
}

