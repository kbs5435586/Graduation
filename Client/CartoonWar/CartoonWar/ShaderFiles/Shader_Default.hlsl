
cbuffer cbPerObject : register(b0)
{
	float4x4 matWorld;
	float4x4 matView;
	float4x4 matProj;
};

struct VS_IN
{
	float3	vPos : POSITION;
	float4	vColor : COLOR;
};

struct VS_OUT
{
	float4	vPos : SV_POSITION;
	float4	vColor : COLOR;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPos = mul(mul(mul(float4(vIn.vPos, 1.0f), matWorld), matView), matProj);
	vOut.vColor = vIn.vColor;

	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	return vIn.vColor;
}
