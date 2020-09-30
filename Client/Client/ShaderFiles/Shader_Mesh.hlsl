
cbuffer cbPerObject : register(b0)
{
	float4x4 matWorld;
	float4x4 matView;
	float4x4 matProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), matWorld), matView), matProj);
	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	return float4(1.f,1.f,1.f,1.f);
}

