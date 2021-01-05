#pragma once
typedef struct tagCamera_Desc
{
	D3DXVECTOR3		vEye; 
	D3DXVECTOR3		vAt;
	D3DXVECTOR3		vAxisY; 
}CAMERADESC;

typedef struct tagProjection_Desc
{
	float		fFovY; 
	float		fAspect;
	float		fNear;
	float		fFar;
}PROJDESC;

typedef struct tagVertex_Texture
{
	D3DXVECTOR3			vPosition;
	D3DXVECTOR2			vTexUV;
}VTXTEX;

typedef struct tagVertex_Normal_Texture
{
	D3DXVECTOR3			vPosition;
	D3DXVECTOR3			vNormal;
	D3DXVECTOR2			vTexUV;
}VTXNORTEX;

typedef struct tagMesh_Texture
{
	LPDIRECT3DTEXTURE9			pDiffuseTexture;
	LPDIRECT3DTEXTURE9			pNormalTexture;
	LPDIRECT3DTEXTURE9			pSpecularTexture;
}MESHTEXTURE;

typedef struct tagSubset_Desc
{
	D3DXMATERIAL		Material;
	MESHTEXTURE			MeshTexture;
}SUBSETDESC;

typedef struct tagVertex_Color
{
	D3DXVECTOR3			vPosition;
	DWORD				dwColor;
}VTXCOL;

typedef struct tagVertex_ViewPort
{
	D3DXVECTOR4			vPosition;
	D3DXVECTOR2			vTexUV;
}VTXVIEWPORT;

typedef struct tagVertex_Cube_Texture
{
	D3DXVECTOR3			vPosition;
	D3DXVECTOR3			vTexUV;
}VTXCUBETEX;

typedef struct tagPolygon16
{
	unsigned short		_0, _1, _2;
}POLYGON16;

typedef struct tagPolygon32
{
	unsigned long		_0, _1, _2;
}POLYGON32;

typedef struct tagD3DXMeshContainer_Derived : public D3DXMESHCONTAINER
{
	LPD3DXMESH			pOriginalMesh;
	// d3dxmaterial + texture
	SUBSETDESC*			pSubSetDesc;
	unsigned long		dwNumFrames;
	D3DXMATRIX**		ppCombinedTransformationMatrices;
	D3DXMATRIX*			pOffsetMatrices;
	D3DXMATRIX*			pRenderingMatrices; // pOffset * pCombined

}D3DXMESHCONTAINER_DERIVED;

typedef struct tagD3DXFrame_Derived : public D3DXFRAME
{
	D3DXMATRIX		CombinedTransformationMatrix;
}D3DXFRAME_DERIVED;

typedef struct tagMapInfo
{
	tagMapInfo() {};
	tagMapInfo(TCHAR* pChar, float fSizeX, float fSizeZ, float fInterval)
	{
		lstrcpy(m_strKey, pChar);
		m_fSizeX = fSizeX;
		m_fSizeZ = fSizeZ;
		m_fInterval = fInterval;
	}
	TCHAR	m_strKey[128];
	float	m_fSizeX;
	float	m_fSizeZ;
	float	m_fInterval;
}MAPINFO;


typedef struct tagColliderInfo
{
	BOX_TYPE			eBoxType;
	D3DXMATRIX				matLocalTransform;
	const D3DXMATRIX*		pBoneMatrix;
	const D3DXMATRIX*		pWorldMatrix;
	tagColliderInfo() {	}
	tagColliderInfo(BOX_TYPE InBoxType, D3DXMATRIX InLocalTransform
		, const D3DXMATRIX* pInBoneMatrix, const D3DXMATRIX* pInWorldMatrix)
		: eBoxType(InBoxType), matLocalTransform(InLocalTransform)
		, pBoneMatrix(pInBoneMatrix), pWorldMatrix(pInWorldMatrix)
	{ }
}COLLIDER_INFO;