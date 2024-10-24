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
	XMMATRIX	matWorld;
	XMMATRIX	matView;
	XMMATRIX	matProj;
	XMMATRIX	matWV;
	XMMATRIX	matWVP;
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


typedef struct tagLight
{
	LIGHT_TYPE	eLightType;
	XMFLOAT4	vDiffuse;
	XMFLOAT4	vSpecular;
	XMFLOAT4	vAmbient;
	XMFLOAT4	vDirection;
	float		fRange;
	float		fPower;
	tagLight() { }
	tagLight(LIGHT_TYPE eType, XMFLOAT4 vDiff, XMFLOAT4 vSpec, XMFLOAT4 vAmbi, XMFLOAT4 vDir, float fRan)
	{
		eLightType = eType;
		vDiffuse = vDiff;
		vSpecular = vSpec;
		vAmbient = vAmbi;
		vDirection = vDir;
		fRange = fRan;
	}
	tagLight(tagLight* tOther)
	{
		eLightType = tOther->eLightType;
		vDiffuse = tOther->vDiffuse;
		vSpecular = tOther->vSpecular;
		vAmbient = tOther->vAmbient;
		vDirection = tOther->vDirection;
		fRange = tOther->fRange;
		fPower = tOther->fPower;
	}

}LIGHT;



