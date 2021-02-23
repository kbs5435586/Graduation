#ifndef _VALUE
#define _VALUE


typedef struct tagLightColor
{
	float4				vDiffuse;
	float4				vSpecular;
	float4				vAmbient;
}LIGHT;

typedef struct tagLightInfo
{
	tagLightColor		tColor;
	float4				vLightPos;
	float4				vLightDir;
	float				fRange;
	float				fAngle;
	float				fPower;
	int					iLightType;
	int					iPadding;
}LIGHTINFO;


cbuffer	TRANSFORM_MATRIX : register (b0)
{
	row_major matrix	matWorld;
	row_major matrix	matView;
	row_major matrix	matProj;

	row_major matrix	matWV;
	row_major matrix	matWVP;
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
	LIGHTINFO			tLight;
	int					iNumLight;
	float3				iPaddingLight;
};


Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
Texture2D g_texture2 : register(t2);
Texture2D g_texture3 : register(t3);
Texture2D g_texture4 : register(t4);
Texture2D g_texture5 : register(t5);
Texture2D g_texture6 : register(t6);
Texture3D g_texture7 : register(t7);



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

SamplerState Sampler0 : register(s0);
SamplerState Sampler1 : register(s1);



cbuffer TEMP :register(b3)
{
	int i;
}
#endif