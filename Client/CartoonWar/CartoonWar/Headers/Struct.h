#pragma once

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
	tagMesh(XMFLOAT3 vPos, XMFLOAT3 vNor, XMFLOAT2 UV, XMFLOAT3 Tangent, XMFLOAT3 Binormal)
	{
		vPosition = vPos;
		vNormal = vNor;
		vUV = UV;
		vTangent = Tangent;
		vBinormal = Binormal;
	}
	XMFLOAT3		vPosition;
	XMFLOAT3		vNormal;
	XMFLOAT2		vUV;
	XMFLOAT3		vTangent;
	XMFLOAT3		vBinormal;

	bool operator==(const tagMesh& other) const
	{
		if (vPosition.x != other.vPosition.x || vPosition.y != other.vPosition.y || vPosition.z != other.vPosition.z)
			return false;

		if (vNormal.x != other.vNormal.x || vNormal.y != other.vNormal.y || vNormal.z != other.vNormal.z)
			return false;

		if (vUV.x != other.vUV.x || vUV.y != other.vUV.y)
			return false;

		if (vTangent.x != other.vTangent.x || vTangent.y != other.vTangent.y || vTangent.z != other.vTangent.z)
			return false;

		if (vBinormal.x != other.vBinormal.x || vBinormal.y != other.vBinormal.y || vBinormal.z != other.vBinormal.z)
			return false;

		return true;
	}
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
	XMFLOAT4X4	matWorld;
	XMFLOAT4X4	matView;
	XMFLOAT4X4	matProj;
	XMFLOAT4X4	matWV;
	XMFLOAT4X4	matWVP;
}MAINPASS;

typedef struct tagMaterial
{
	XMFLOAT4		vMtrlDiff;
	XMFLOAT4		vMtrlSpec;
	XMFLOAT4		vMtrlAmb;
	XMFLOAT4		vMtrlEmiv;
}MATERIAL;

typedef struct tagMtrInfo
{
	XMFLOAT4		vMtrlDiff = { 1.f, 1.f,1.f, 1.f };
	XMFLOAT4		vMtrlSpec = { 1.f, 1.f,1.f, 1.f };;
	XMFLOAT4		vMtrlAmb = { 1.f, 1.f,1.f, 1.f };;
	XMFLOAT4		vMtrlEmiv = { 1.f, 1.f,1.f, 1.f };

	wstring			strMtrlName;
	wstring			strDiff;
	wstring			strNormal;
	wstring			strSpec;
}MTRLINFO;

struct RenderInfo
{
	string						strNodeName;
	vector<MESH>				vecVertices;
	vector<size_t>				vecIndices;
	vector<tagMtrInfo>			vecMtrlInfo;
	D3D12_VERTEX_BUFFER_VIEW	VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		IndexBufferView;
};


typedef struct tagLightColor
{
	XMFLOAT4	vDiffuse;
	XMFLOAT4	vSpecular;
	XMFLOAT4	vAmbient;
}LIGHTCOLOR;

typedef struct tagLight
{
	LIGHTCOLOR	tLightColor;
	XMFLOAT4	vLightPos;
	XMFLOAT4	vLightDir;
	float		fRange;
	float		fAngle;
	float		fPower;
	int			iLightType;
	int			iPadding;

}LIGHT;

typedef struct tagLightInfo
{
	LIGHT			arrLight;
	int				iCurLightCnt;
	XMFLOAT3		vPadding;
}LIGHTINFO;

typedef struct tagRenderTarget
{
	ID3D12Resource*		pTarget;
	XMFLOAT4			fClear_Color;
}RENDERTARGER;

typedef struct tagReflect
{
	XMFLOAT4X4	matReflect;
}REFLECT;