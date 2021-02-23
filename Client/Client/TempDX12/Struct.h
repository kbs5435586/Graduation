#pragma once
#include "protocol.h"

typedef struct tagVertexCol
{
	XMFLOAT3 vPosition;
	XMFLOAT4 vColor;
	tagVertexCol()
	{

	}
	tagVertexCol(XMFLOAT3 vPos, XMFLOAT4 vColor_)
	{
		vPosition = vPos;
		vColor = vColor_;
	}
}VTXCOL;

typedef struct tagVertexTex
{
	XMFLOAT3 vPosition;
	XMFLOAT2 vTex;
	tagVertexTex()
	{

	}
	tagVertexTex(XMFLOAT3 vPos, XMFLOAT2 vTex_)
	{
		vPosition = vPos;
		vTex = vTex_;
	}
}VTXTEX;

typedef struct tagVertexTexCube
{
	XMFLOAT3 vPosition;
	XMFLOAT3 vTex;
	tagVertexTexCube()
	{

	}
	tagVertexTexCube(XMFLOAT3 vPos, XMFLOAT3 vTex_)
	{
		vPosition = vPos;
		vTex = vTex_;
	}
}VTXTEXCUBE;

typedef struct tagVertexTexNor
{
	XMFLOAT3	vPos = {};
	XMFLOAT3	vNormal = {};
	XMFLOAT2	vTexUV = {};
	tagVertexTexNor() {}
	tagVertexTexNor(XMFLOAT3 _vPos, XMFLOAT3 _vNormal, XMFLOAT2 _vTexUV)
	{
		vPos = _vPos;
		vNormal = _vNormal;
		vTexUV = _vTexUV;
	}

}VTXTEXNOR;


typedef struct tagMesh
{
	tagMesh()
	{

	}

	tagMesh(XMFLOAT3 vPos, XMFLOAT3 vNor, XMFLOAT2 UV)
	{
		vPosition = vPos;
		vNormal = vNor;
		vUV = UV;
	}
	XMFLOAT3		vPosition;
	XMFLOAT3		vNormal;
	//XMFLOAT3		vTangent;
	//XMFLOAT3		vBinormal;
	XMFLOAT2		vUV;
}MESH;

typedef struct tagCamera_Desc
{
	XMFLOAT3		vEye;
	XMFLOAT3		vAt;
	XMFLOAT3		vAxisY;
}CAMERADESC;

typedef struct tagProjection_Desc
{
	float		fFovY;
	float		fAspect;
	float		fNear;
	float		fFar;
}PROJDESC;


typedef struct tagMainPass
{
	XMMATRIX	matWorld;
	XMMATRIX	matView;
	XMMATRIX	matProj;

}MAINPASS;


typedef struct tagMainPass_Light
{
	XMMATRIX	matWorld;
	XMMATRIX	matView;
	XMMATRIX	matProj;
	// CamPos
	XMFLOAT4	vCameraPos;
	// Material
	XMFLOAT4	vMaterialDiffuse;
	XMFLOAT4	vMaterialSpecular;
	XMFLOAT4	vMaterialAmbient;
	// Light
	XMFLOAT4	vLightDiffuse;
	XMFLOAT4	vLightSpecular;
	XMFLOAT4	vLightAmbient;
	XMFLOAT4	vLightDirection;
	float		fPower;
	// CamPos

}MAINPASS_LIGHT;



typedef struct tagMtrInfo
{
	XMFLOAT4		vMtrlDiff = { 1.f, 1.f,1.f, 1.f };
	XMFLOAT4		vMtrlSpec = { 1.f, 1.f,1.f, 1.f };;
	XMFLOAT4		vMtrlAmb = { 1.f, 1.f,1.f, 1.f };;
	XMFLOAT4		vMtrlEmiv = { 1.f, 1.f,1.f, 1.f };
	float			fPower = 0.f;

	wstring			strMtrlName;
	wstring			strDiff;
	wstring			strNormal;
	wstring			strSpec;
}MTRLINFO;


struct VTXTEMP
{
	XMFLOAT3		vPos;
	XMFLOAT3		vNormal;
	XMFLOAT2		vUV;
};


struct RenderInfo
{
	unsigned int				iIndices = 0;
	unsigned int				iVertices = 0;

	vector<MESH>				vecMeshData;
	vector<XMFLOAT3>			vecPosition;
	vector<XMFLOAT3>			vecTangent;
	vector<XMFLOAT3>			vecBinormal;
	vector<XMFLOAT3>			vecNormal;
	vector<XMFLOAT2>			vecUV;


	vector<int>					vecIndices;
	D3D12_VERTEX_BUFFER_VIEW	VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		IndexBufferView;
	//XMFLOAT4* vPos;

	vector<tagMtrInfo>			vecMtrlInfo;

	VTXTEMP*						temp;
};


typedef struct tagLight
{
	LIGHT_TYPE	eLightType;
	XMFLOAT4	vDiffuse;
	XMFLOAT4	vSpecular;
	XMFLOAT4	vAmbient;
	XMFLOAT4	vDirection;

}LIGHT;

struct Player_Info
{
	bool showCharacter;
	char name[MAX_ID_LEN + 1];
	int x = 0;
	int y = 0;
};




