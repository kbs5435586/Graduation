#pragma once

typedef struct tagVertexCol
{
	_vec3 vPosition;
	_vec4 vColor;
	_vec3 vNormal;
	tagVertexCol()
	{

	}
	tagVertexCol(_vec3 vPos, _vec4 vColor_, _vec3 vNormal_)
	{
		vPosition = vPos; 
		vColor = vColor_;
		vNormal = vNormal_;
	}
}VTXCOL;

typedef struct tagVertexTex
{
	_vec3	vPosition;
	_vec2	vTex;
	tagVertexTex()
	{

	}
	tagVertexTex(_vec3 vPos, XMFLOAT2 vTex_)
	{
		vPosition = vPos;
		vTex = vTex_;
	}
}VTXTEX;

typedef struct tagVertexTexCube
{
	_vec3 vPosition;
	_vec3 vTex;
	tagVertexTexCube()
	{

	}
	tagVertexTexCube(_vec3 vPos, _vec3 vTex_)
	{
		vPosition = vPos;
		vTex = vTex_;
	}
}VTXTEXCUBE;

typedef struct tagVertexTexNorCube
{
	_vec3	vPosition;
	_vec3	vTex;
	_vec3	vNormal;
	tagVertexTexNorCube()
	{

	}
	tagVertexTexNorCube(_vec3 vPos, _vec3 vTex_, _vec3 vNor_)
	{
		vPosition = vPos;
		vTex = vTex_;
		vNormal = vNor_;
	}
}VTXTEXCUBENOR;

typedef struct tagVertexTex2NorCube
{
	_vec3	vPosition;
	_vec2	vTex;
	_vec3	vNormal;
	tagVertexTex2NorCube()
	{

	}
	tagVertexTex2NorCube(_vec3 vPos, _vec2 vTex_, _vec3 vNor_)
	{
		vPosition = vPos;
		vTex = vTex_;
		vNormal = vNor_;
	}
}VTXTEX2CUBENOR;

typedef struct tagVertexTexNor
{
	_vec3	vPos = {};
	_vec3	vNormal = {};
	_vec2	vTexUV = {};
	tagVertexTexNor() {}
	tagVertexTexNor(_vec3 _vPos, _vec3 _vNormal, _vec2 _vTexUV)
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
	tagMesh(_vec3 vPos, _vec3 vNor, _vec2 UV)
	{
		vPosition = vPos;
		vNormal = vNor;
		vUV = UV;
	}
	tagMesh(_vec3 vPos, _vec3 vNor, _vec2 UV, _vec3 Tangent, _vec3 Binormal)
	{
		vPosition = vPos;
		vNormal = vNor;
		vUV = UV;
		vTangent = Tangent;
		vBinormal = Binormal;
	}
	_vec3		vPosition;
	_vec4		vColor;
	_vec2		vUV;
	_vec3		vNormal;
	_vec3		vTangent;
	_vec3		vBinormal;
	_vec4		vWeight;
	_vec4		vIndices;

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
	_vec3		vEye;
	_vec3		vAt;
	_vec3		vAxisY;
}CAMERADESC;

typedef struct tagProjection_Desc
{
	float		fFovY;
	float		fAspect;
	float		fNear;
	float		fFar;
}PROJDESC;

typedef struct tagTextureInfo
{
	float		fFrameTime;
	_vec3	vScrollSpeed;
	_vec3	vScale;
	float		fPadding;
}TEXINFO;

typedef struct tagMainPass
{
	_matrix	matWorld;
	_matrix	matView;
	_matrix	matProj;
	_matrix	matWV;
	_matrix	matWVP;
	_matrix	matProjInv;
	_matrix	matViewInv;
	_matrix	matRev;

	_vec4	vCamPos;
	_vec4	vLook;

	_matrix	matLightView;
	_matrix	matLightProj;
}MAINPASS;

typedef struct tagMaterial
{
	_vec4		vMtrlDiff;
	_vec4		vMtrlSpec;
	_vec4		vMtrlAmb;
	_vec4		vMtrlEmiv;
}MATERIAL;

typedef struct tagMtrInfo
{
	_vec4		vMtrlDiff = { 1.f, 1.f,1.f, 1.f };
	_vec4		vMtrlSpec = { 1.f, 1.f,1.f, 1.f };;
	_vec4		vMtrlAmb = { 1.f, 1.f,1.f, 1.f };;
	_vec4		vMtrlEmiv = { 1.f, 1.f,1.f, 1.f };

	wstring			strMtrlName;
	wstring			strDiff;
	wstring			strNormal;
	wstring			strSpec;
}MTRLINFO;

struct Indices
{
	ComPtr<ID3D12Resource>		pIB;
	int							iIndexCnt;
	DXGI_FORMAT					eFormat;
	void* pSystem;
	D3D12_INDEX_BUFFER_VIEW		IndexBufferView;
};

struct RenderInfo
{
	string						strNodeName;
	vector<tagMtrInfo>			vecMtrlInfo;
	D3D12_VERTEX_BUFFER_VIEW	VertexBufferView;

	ComPtr<ID3D12Resource>		pVB;

	int							iVtxCnt;
	int							iVtxSize;
	vector<MESH>				vecVertices;
	vector<Indices>				vecIndices;
	class CTexture*				pTexture;
};

typedef struct tagRenderSupport
{
	tagRenderSupport() {}
	tagRenderSupport(int iIncicesCnt_, D3D12_VERTEX_BUFFER_VIEW VertexBufferView_, D3D12_INDEX_BUFFER_VIEW		IndexBufferView_)
	{
		iIncicesCnt = iIncicesCnt_;
		VertexBufferView = VertexBufferView_;
		IndexBufferView = IndexBufferView_;
	}
	int							iIncicesCnt;
	D3D12_VERTEX_BUFFER_VIEW	VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		IndexBufferView;
}RENDERSUP;


typedef struct tagLightColor
{
	_vec4	vDiffuse;
	_vec4	vSpecular;
	_vec4	vAmbient;
}LIGHTCOLOR;

typedef struct tagLight
{
	LIGHTCOLOR	tLightColor;
	_vec4		vLightPos;
	_vec4		vLightDir;
	int			iLightType;
	float		fRange;
	float		fAngle;
	int			iPadding;

}LIGHT;

typedef struct tagLightInfo
{
	LIGHT			arrLight[100];
	int				iCurLightCnt;
	_vec3			vPadding;
}LIGHTINFO;

typedef struct tagRenderTarget
{
	ID3D12Resource* pTarget;
	_vec4			fClear_Color;
}RENDERTARGER;

typedef struct tagReflect
{
	_matrix	matReflect;
}REFLECT;



typedef struct tagInfo
{
	tagInfo() {};
	tagInfo(float hp, float mp, float att, float def)
	{
		fHP = hp;
		fMaxHP = fHP;
		fMP = mp;
		fAtt = att;
		fDef = def;
	};
	float	fHP = 0.f;
	float	fMaxHP = 0.f;
	float	fMP = 0.f;
	float	fAtt = 0.f;
	float	fDef = 0.f;
}INFO;


typedef struct tagDistortion
{
	XMFLOAT2	fDistortion1;
	XMFLOAT2	fDistortion2;
	XMFLOAT2	fDistortion3;
	float		fDistortionScale;
	float		fDistortionBias;
}DISTORTION;



typedef struct tagFog
{
	float	fFogStart;
	float	fFogEnd;
}FOG;


typedef struct tagTime
{
	float		fTime;
}TIME;


struct VTX
{
	XMFLOAT3 vPos;
	XMFLOAT4 vColor;
	XMFLOAT2 vUV;

	XMFLOAT3 vTangent;
	XMFLOAT3 vNormal;
	XMFLOAT3 vBinormal;

	XMFLOAT4 vWeights;
	XMFLOAT4 vIndices;
};



//////////////////////////MESH///////////////////////

struct tKeyFrame
{
	FbxAMatrix  matTransform;
	double		dTime;
};

struct tBone
{
	wstring				strBoneName;
	int					iDepth;			// 계층구조 깊이
	int					iParentIndx;	// 부모 Bone 의 인덱스
	FbxAMatrix			matOffset;		// Offset 행렬( -> 뿌리 -> Local)
	FbxAMatrix			matBone;
	vector<tKeyFrame>	vecKeyFrame;
};

struct tAnimClip
{
	wstring		strName;
	FbxTime		tStartTime;
	FbxTime		tEndTime;
	FbxLongLong	llTimeLength;
	FbxTime::EMode eMode;
};



typedef struct _tagMtrlInfo
{
	XMFLOAT4	vMtrlDiff;
	XMFLOAT4	vMtrlSpec;
	XMFLOAT4	vMtrlAmb;
	XMFLOAT4	vMtrlEmiv;
	_tagMtrlInfo()
		: vMtrlDiff(1.f, 1.f, 1.f, 1.f)
		, vMtrlSpec(1.f, 1.f, 1.f, 1.f)
		, vMtrlAmb(1.f, 1.f, 1.f, 1.f)
		, vMtrlEmiv(1.f, 1.f, 1.f, 1.f)
	{}

}tMtrlInfo;

typedef struct _tagFbxMat
{
	tMtrlInfo	tMtrl;
	wstring     strMtrlName;
	wstring     strDiff;
	wstring		strNormal;
	wstring		strSpec;
}tFbxMaterial;

typedef struct _tagWeightsAndIndices
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
	bool								bAnimation;
	vector<vector<tWeightsAndIndices>>	vecWI;

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

}tContainer;



struct tMTKeyFrame
{
	double		dTime;
	int			iFrame;
	XMFLOAT3	vTranslate;
	XMFLOAT3	vScale;
	XMFLOAT4	qRot;
};

struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	_matrix				matOffset;	// Offset 행렬(뼈 -> 루트 까지의 행렬)
	_matrix				matBone;   // 이거 안씀
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring		strAnimName;
	int			iStartFrame;
	int			iEndFrame;
	int			iFrameLength;

	double		dStartTime;
	double		dEndTime;
	double		dTimeLength;
	float		fUpdateTime; // 이거 안씀

	FbxTime::EMode eMode;
};


typedef struct tagReplenishment
{
	int				m_arrInt[(UINT)SHADER_PARAM::INT_END - (UINT)SHADER_PARAM::INT_0];
	float			m_arrFloat[(UINT)SHADER_PARAM::FLOAT_END - (UINT)SHADER_PARAM::FLOAT_0];
	XMFLOAT2		m_arrVec2[(UINT)SHADER_PARAM::VEC2_END - (UINT)SHADER_PARAM::VEC2_0];
	XMFLOAT4		m_arrVec4[(UINT)SHADER_PARAM::VEC4_END - (UINT)SHADER_PARAM::VEC4_0];
	Matrix			m_arrMat[(UINT)SHADER_PARAM::MATRIX_END - (UINT)SHADER_PARAM::MATRIX_0];
}REP;




typedef struct tagParticle
{
	XMFLOAT3 vWorldPos;		// 위치	
	XMFLOAT3 vWorldDir;		// 이동 방향

	float m_fCurTime;	// 현재 시간
	float m_fLifeTime;	// 최대 생명주기

	int  iAlive;
	int  arrPading[3];
}PARTICLE;

typedef struct tagParticleShared
{
	int iAddCount;
	int arrPading[3];
}PARTICLESHARED;


typedef struct tagGlobal
{
	int		iWincx;
	int		iWincy;
	int		iTeam;
	float	fTimeDelta;
	float	fAccTime = 0.f;
}GLOBAL;


struct tFrameTrans
{
	XMFLOAT4	vTranslate;
	XMFLOAT4	vScale;
	XMFLOAT4	qRot;
};

//struct Player_Info
//{
//	int		id;
//	float	x;
//	float	y;
//	float	z;
//	char	name[MAX_ID_LEN + 1];
//	bool	showCharacter;
//};



typedef struct tagAnimController
{
	tagAnimController() {}
	tagAnimController(int _iStartFrm, int _iEndFrm, float _fStartTime, float _fEndTime, bool IsOnce = false)
	{
		iStartFrm = _iStartFrm;
		iEndFrm = _iEndFrm;
		fStartTime = _fStartTime;
		fEndTime = _fEndTime;
		iCurFrm = iStartFrm;
		IsPlayOnce = IsOnce;

		fOriginStartTime = fStartTime;
		fOriginEndTime = fEndTime;
	}
	int		iStartFrm;
	int		iEndFrm;
	int		iCurFrm;
	float	fStartTime;
	float	fCurTime = 0.f;
	float	fEndTime;
	bool	IsPlayOnce = false;

	float	fOriginStartTime;
	float	fOriginEndTime;
}AnimCtrl;


typedef struct tagOBB
{
	Vector3			vPoint[8];
	Vector3			vCenter;
	Vector3			vAlignAxis[3];
	Vector3			vProjAxis[3];
}OBB;


typedef struct tagFlag
{
	Vector3		vPos;
	int			iNum;
}FLAG;



typedef struct tagPlayer
{
	SPECIES		eSpecies;
	COLOR		eColor;
}PLAYER;


typedef struct tagParticleSet
{
	Vector3		vPos;
	int			iMaxParticle;
	float		iMinLifeTime;
	float		fMaxLifeTime;
	float		fStartScale;
	float		fEndScale;
	float		fMinSpeed;
	float		fMaxSpeed;
}PARTICLESET;