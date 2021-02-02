Texture2D    g_texture : register(t1);
SamplerState DiffuseSampler  : register(s0);
struct VS_IN
{
	float3	vPos : POSITION;
	float2	vTexUV : TEXCOORD;
};

struct VS_OUT
{
	float4	vPos : SV_POSITION;
	float2	vTexUV : TEXCOORD;
};


VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT	vOut;

	vOut.vPos = float4(vIn.vPos, 1.f);
	vOut.vTexUV = vIn.vTexUV;

	return vOut;
}


float4	PS_Main(VS_OUT vIn) : SV_Target
{
	return g_texture.Sample(DiffuseSampler, vIn.vTexUV);
}

