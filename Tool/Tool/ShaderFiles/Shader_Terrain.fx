
matrix		g_matWorld, g_matView, g_matProj;

vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightSpecular;
vector		g_vLightAmbient;

vector		g_vMtrlSpecular;
vector		g_vMtrlAmbient;

float		g_fPower;

vector		g_vCamPosition;

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
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD;
};

struct VS_OUT_PHONG
{
	vector	vPosition : POSITION;
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	vector	vWorldPos : TEXCOORD1;
};

VS_OUT_PHONG VS_MAIN_PHONG(VS_IN In)
{
	VS_OUT_PHONG			Out = (VS_OUT_PHONG)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);

	return Out;
}

struct PS_IN_PHONG
{
	vector	vPosition : POSITION;
	vector	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	vector	vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector	vColor : COLOR;
};


PS_OUT PS_MAIN_PHONG(PS_IN_PHONG In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vDiffuseColor = tex2D(DiffuseSampler, In.vTexUV * 60.0f);

	vector	vShade;
	vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	vector	vSpecular;
	vector	vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector	vLook = In.vWorldPos - g_vCamPosition;

	vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), g_fPower);

	Out.vColor = ((g_vLightDiffuse * vDiffuseColor) + (g_vLightSpecular * g_vMtrlSpecular) * vSpecular) * saturate(vShade + (g_vLightAmbient * g_vMtrlAmbient));

	Out.vColor.a = 1.f;

	return Out;
}

technique	DefaultDevice
{
	pass Phong // ÇÈ¼¿´Ü¿¡¼­ ºû ¿¬»ê.
	{
	
		VertexShader = compile vs_3_0 VS_MAIN_PHONG();
		PixelShader = compile ps_3_0 PS_MAIN_PHONG();
	}
}
