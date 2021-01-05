#pragma once

matrix		g_matWorld, g_matView, g_matProj;

texture		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct VS_IN
{
	float3	vPosition : POSITION;
	float3	vTexUV : TEXCOORD;
};

struct VS_OUT
{
	vector	vPosition : POSITION;
	float3	vTexUV : TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	vector	vPosition : POSITION;
	float3	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : COLOR;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out;

	Out.vColor = texCUBE(DiffuseSampler, In.vTexUV);	

	return Out;
}

technique	Default_Device
{
	pass Default_Rendering
	{
		CullMode = cw;
		ZEnable = false;
		ZWriteEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}	
}