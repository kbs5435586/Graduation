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

typedef struct tagDynamic_MeshRender
{
	D3D12_VERTEX_BUFFER_VIEW	tVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		tIndexBufferView;
	unsigned int				iNumOfIndices;
}DYNAMIC_MESH_RENDER;

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;

	bool operator==(const Vertex& other) const
	{
		if (Pos.x != other.Pos.x || Pos.y != other.Pos.y || Pos.z != other.Pos.z)
			return false;

		if (Normal.x != other.Normal.x || Normal.y != other.Normal.y || Normal.z != other.Normal.z)
			return false;

		if (TexC.x != other.TexC.x || TexC.y != other.TexC.y)
			return false;

		return true;
	}
	bool operator<(const Vertex& rhs)const
	{
		if (Pos.x < rhs.Pos.x || Pos.y < rhs.Pos.y || Pos.z < rhs.Pos.z)
			return false;
		return true;
	}
};
struct CharacterVertex : Vertex
{
	DirectX::XMFLOAT3 BoneWeights;
	BYTE BoneIndices[4];

	uint16_t MaterialIndex;
};
struct UIVertex : Vertex
{
	float Row;
};


struct Material
{
	// Unique material name for lookup.
	std::string Name;

	// Index into constant buffer corresponding to this material.
	int MatCBIndex = -1;

	// Index into SRV heap for diffuse texture.
	int DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int NormalSrvHeapIndex = -1;

	// Dirty flag indicating the material has changed and we need to update the constant buffer.
	// Because we have a material constant buffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify a material we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = 2;

	// Material constant buffer data used for shading.
	DirectX::XMFLOAT3 Ambient = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	DirectX::XMFLOAT3 Specular = { 0.01f, 0.01f, 0.01f };
	DirectX::XMFLOAT3 Emissive = { 0.01f, 0.01f, 0.01f };

	float Roughness = .25f;
	DirectX::XMFLOAT4X4 MatTransform = Matrix::Identity();
};


struct BoneIndexAndWeight
{
	BYTE mBoneIndices;
	float mBoneWeight;

	bool operator < (const BoneIndexAndWeight& rhs)
	{
		return (mBoneWeight > rhs.mBoneWeight);
	}
};

struct CtrlPoint
{
	DirectX::XMFLOAT3 mPosition;
	std::vector<BoneIndexAndWeight> mBoneInfo;
	std::string mBoneName;

	CtrlPoint()
	{
		mBoneInfo.reserve(4);
	}

	void SortBlendingInfoByWeight()
	{
		std::sort(mBoneInfo.begin(), mBoneInfo.end());
	}
};
typedef struct tagMtrInfo
{
	XMFLOAT4		vMtrlDiff = { 1.f, 1.f,1.f, 1.f };
	XMFLOAT4		vMtrlSpec = { 1.f, 1.f,1.f, 1.f };;
	XMFLOAT4		vMtrlAmb = { 1.f, 1.f,1.f, 1.f };;
	XMFLOAT4		vMtrlEmiv = { 1.f, 1.f,1.f, 1.f };
}tMtrlInfo;

typedef struct tagFbxMat
{
	tMtrlInfo		tMtrl;
	wstring			strMtrlName;
	wstring			strDiff;
	wstring			strNormal;
	wstring			strSpec;
}tFbxMaterial;

typedef struct tagWeightAndIndices
{
	int		iBoneIdx;
	double	dWeight;
}tWeightsAndIndices;

typedef struct _tagContainer
{
	wstring								strName;
	vector<XMFLOAT3>					vecPos;
	vector<XMFLOAT3>					vecTangent;
	vector<XMFLOAT3>					vecBinormal;
	vector<XMFLOAT3>					vecNormal;
	vector<XMFLOAT2>					vecUV;

	vector<XMFLOAT4>					vecIndices;
	vector<XMFLOAT4>					vecWeights;

	vector<vector<UINT>>				vecIdx;
	vector<tFbxMaterial>				vecMtrl;

	// Animation 관련 정보
	bool                        bAnimation;
	vector<vector<tWeightsAndIndices>>   vecWI;

	void Resize(UINT _iSize)
	{
		vecPos.resize(_iSize);
		vecTangent.resize(_iSize);
		vecBinormal.resize(_iSize);
		vecNormal.resize(_iSize);
		vecUV.resize(_iSize);
		vecIndices.resize(_iSize);
		vecWeights.resize(_iSize);
		vecWI.resize(_iSize);
	}

	_tagContainer()
	{

	}

}tContainer;


struct MeshFromFbx
{
	D3D12_PRIMITIVE_TOPOLOGY			topology;
	size_t								slot;
	size_t								offset;
	size_t								vertices;
	ID3D12Resource*						pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			VertexBufferView;
	ID3D12Resource*						pIndexBuffer;
	ID3D12Resource*						pIndexUploadeBuffer;
	D3D12_VERTEX_BUFFER_VIEW			IndexBufferView;
};

struct MeshData
{
	MeshData()
	{

	}
	MeshData(int _iNumVertices, int _iIndeices, int* _pIndices, vector< XMFLOAT3> _vPos)
	{
		iNumVertices = _iNumVertices;
		iIndices = _iIndeices;
		pIndices = _pIndices;
		vecPosition = _vPos;
	}
	int		iNumVertices;
	int		iIndices;
	int*	pIndices;
	vector< XMFLOAT3>	vecPosition;
};

struct RenderInfo
{
	unsigned int				iIndices = 0;
	D3D12_VERTEX_BUFFER_VIEW	VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		IndexBufferView;
};