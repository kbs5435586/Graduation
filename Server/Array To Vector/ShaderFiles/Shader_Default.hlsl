
cbuffer cbPerObject : register(b0)
{
	float4x4 matWorld;
	float4x4 matView;
	float4x4 matProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), matWorld), matView), matProj);
	vout.Color = vin.Color;

	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	return pin.Color;
}

