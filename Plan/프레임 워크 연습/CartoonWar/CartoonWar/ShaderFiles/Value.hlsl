#ifndef _VALUE
#define _VALUE


struct LIGHT
{
	float4				vDiffuse;
	float4				vSpecular;
	float4				vAmbient;
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

	float4				vCamPos;

	row_major matrix	I_matView;
	row_major matrix	I_matProj;

	row_major matrix	I_matWV;
	row_major matrix	I_matWVP;

	float4				I_vCamPos;
};

cbuffer MATERIAL : register (b1)
{
	float4				vMaterial_Diffuse;
	float4				vMaterial_Specular;
	float4				vMaterial_Ambient;
	float4				vMaterial_Emissive;
};

cbuffer LIGHT :register(b2)
{
	LIGHTINFO			tLight[100];
	int					iNumLight;
	float3				iPaddingLight;
};

cbuffer REFLECT : register(b3)
{
	row_major matrix	matReflect;
};


Texture2D	g_texture0		: register(t0);
Texture2D	g_texture1		: register(t1);
Texture2D	g_texture2		: register(t2);
Texture2D	g_texture3		: register(t3);
Texture2D	g_texture4		: register(t4);
Texture2D	g_texture5		: register(t5);
Texture2D	g_texture6		: register(t6);
TextureCube g_textureCube	: register(t7);

SamplerState Sampler0		: register(s0);
SamplerState Sampler1		: register(s1);

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


#endif