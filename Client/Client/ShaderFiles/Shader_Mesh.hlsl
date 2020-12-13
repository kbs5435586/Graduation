
Texture2D    g_texture : register(t0);
SamplerState DiffuseSampler  : register(s0);

cbuffer cbPerObject : register(b0)
{
	float4x4	matWorld;
	float4x4	matView;
	float4x4	matProj;
};

struct VS_IN
{
	float3 vPos  : POSITION;
	float3 vNormal : NORMAL;
	float2 vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPos  : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vUV : TEXCOORD;
};

VS_OUT VS_Main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.vPos = mul(mul(mul(float4(vIn.vPos, 1.0f), matWorld), matView), matProj);
	vOut.vNormal = vIn.vNormal;
	vOut.vUV = vIn.vUV;

	return vOut;
}

float4 PS_Main(VS_OUT vIn) : SV_Target
{
	float4 vColor = g_texture.Sample(DiffuseSampler, vIn.vUV);
	return vColor;
	//return float4(vIn.vUV.x, vIn.vUV.x, 0.f, 1.f);
}


