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


struct	VS_IN
{
	float3	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : SV_POSITION;
	float4	vNormal : NORMAL;
	float2	vTexUV0 : TEXCOORD0;
	float3	vTexWeight123 : TEXCOORD2;

	float4	vWorldPos : TEXCOORD4;
};

float4 GetInverseColor(float4 vColor)
{
	return (float4(1.f, 1.f, 1.f, 1.f) - vColor);
}

VS_OUT	VS_Main(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.vPosition = mul(mul(mul(float4(vIn.vPosition, 1.0f), matWorld), matView), matProj);

	vOut.vTexUV0 = vIn.vTexUV;

	vIn.vNormal = normalize(vIn.vNormal);

	float3 vLightPos = float3(vCamPos.x, vCamPos.y, vCamPos.z);

	float3	vLight = vLightPos - vIn.vPosition;
	vLight = normalize(vLight);

	float	fFactor = (dot(vIn.vNormal, vLight) + 1.f) * 3.f;

	//Calculate Weight
	vOut.vTexWeight123 = float3(1.f - clamp(abs(fFactor - 5.f), 0.f, 1.f),
								1.f - clamp(abs(fFactor - 4.f), 0.f, 1.f),
								1.f - clamp(abs(fFactor - 3.f), 0.f, 1.f));
	return vOut;
}

float4	PS_Main(VS_OUT vIn) : SV_Target
{

	float4	vOutColor = float4(0.f, 0.f,0.f,1.f);

	float4 vColor123 = g_texture.Sample(DiffuseSampler, vIn.vTexUV0);

	float4 vWeight123 = float4(vIn.vTexWeight123, 1.f);


	vColor123 = saturate(dot(GetInverseColor(vColor123), vWeight123));

	vOutColor = saturate(vColor123);
	vOutColor = saturate(GetInverseColor(vOutColor));

	return vOutColor;
}