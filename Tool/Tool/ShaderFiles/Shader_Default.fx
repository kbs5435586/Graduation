

// 컨스턴트 테이블
matrix		g_matWorld, g_matView, g_matProj;
vector		g_vColor;
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
	float2	vTexUV : TEXCOORD;
};

struct VS_OUT
{
	vector	vPosition : POSITION;
	float2	vTexUV : TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

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
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : COLOR;
};
PS_OUT PS_MAIN(VS_OUT In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector	vDiffuseColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor = vDiffuseColor;

	return Out;
}
technique	DefaultDevice
{
	pass Default_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Color_Rendering
	{
		ZEnable = true;
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		CullMode = none;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();

		AlphaBlendEnable = false;
	}	
	pass UI_Rendering
	{
		ZEnable = false;
		ZWriteEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Test_Rendering
	{
		ZEnable = true;
		ZWriteEnable = false;

		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		AlphaRef = 255;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();

		AlphaTestEnable = false;
	}
}