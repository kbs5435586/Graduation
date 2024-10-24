#ifndef _VALUE
#define _VALUE


struct LIGHT
{
	float4				vDiffuse;
	float4				vSpecular;
	float4				vAmbient;
};
struct AD_Light
{
	float4				vDiffuse;
	float4				vSpecular;
	float4				vAmbient;
	float4				vShade;
};


struct LIGHTINFO
{
	LIGHT				tColor;
	float4				vLightPos;
	float4				vLightDir;
	int					iLightType;
	float				fRange;
	float				fAngle;
	int					iPadding;
};


cbuffer	TRANSFORM_MATRIX : register (b0)
{
	row_major matrix	matWorld;
	row_major matrix	matView;
	row_major matrix	matProj;

	row_major matrix	matWV;
	row_major matrix	matWVP;
	row_major matrix	matProjInv;
	row_major matrix	matViewInv;
	row_major matrix	matRev;

	float4				vCamPos;
	float4				vLook;


	row_major matrix	matLightView;
	row_major matrix	matLightProj;
};

cbuffer MATERIAL : register (b1)
{
	float4				vMaterial_Diffuse;
	float4				vMaterial_Specular;
	float4				vMaterial_Ambient;
	float4				vMaterial_Emissive;
};

cbuffer LIGHT_ :register(b2)
{
	LIGHTINFO			tLight[100];
	int					iNumLight;
	float3				iPaddingLight;
};

cbuffer REFLECT : register(b3)
{
	row_major matrix	matReflect;
};

cbuffer TEXINFO : register(b4)
{
	float	fFrameTime;
	float3	vScrollSpeed;
	float3	vScale;
	float	fPadding;
}

cbuffer DISTORTION : register(b5)
{
	float2		fDistortion1;
	float2		fDistortion2;
	float2		fDistortion3;
	float		fDistortionScale;
	float		fDistortionBias;
}

cbuffer FOG : register(b6)
{
	float		fFogStart;
	float		fFogEnd;
}

cbuffer TIME : register(b7)
{
	float		fTime;
}

cbuffer MATERIAL_PARAM : register(b8)
{

	int g_int_0;
	int g_int_1;
	int g_int_2;
	int g_int_3;

	float g_float_0;
	float g_float_1;
	float g_float_2;
	float g_float_3;

	float2 g_vec2_0;
	float2 g_vec2_1;
	float2 g_vec2_2;
	float2 g_vec2_3;

	float4 g_vec4_0;
	float4 g_vec4_1;
	float4 g_vec4_2;
	float4 g_vec4_3;

	row_major float4x4 g_mat_0;
	row_major float4x4 g_mat_1;
	row_major float4x4 g_mat_2;
	row_major float4x4 g_mat_3;

};
struct tFontInfo
{
	int CharNum;
	float left;
	float right;
	int size;
};

Texture2D					g_texture0			: register(t0);
Texture2D					g_texture1			: register(t1);
Texture2D					g_texture2			: register(t2);
Texture2D					g_texture3			: register(t3);
Texture2D					g_texture4			: register(t4);
Texture2D					g_texture5			: register(t5);
Texture2D					g_texture8			: register(t8);
Texture2D					g_texture9			: register(t9);

TextureCube					g_textureCube		: register(t6);    
StructuredBuffer<float4x4>	g_arrFinalBoneMat	: register(t7);


SamplerState				Sampler0			: register(s0);
SamplerState				Sampler1			: register(s1);
SamplerState				Sampler2			: register(s2);
SamplerState				Sampler3			: register(s3);

int HasTex(in Texture2D _tex)
{
	uint iWidth = 0;
	uint iHeight = 0;
	_tex.GetDimensions(iWidth, iHeight);

	if (iWidth == 0 || iHeight == 0)
	{
		return 0;
	}
	return 1;
}




cbuffer GLOBAL : register(b9)
{
	int		wincx;
	int		wincy;
	int		Team;
	float   g_fDT;
	float   g_fAccTime;
	
}


cbuffer BRUSHINFO: register(b10)
{
	float4		vBrushPos;
	float		fBrushRange;
}
const static float Weight[13] =
{
	0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231,
	1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};
const static float Total = 6.2108;






#endif
