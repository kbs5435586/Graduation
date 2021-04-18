#pragma once
#include "Component.h"
class CStructedBuffer;
class CTexture;
class CMesh :
    public CComponent
{
private:
    CMesh();
    CMesh(const CMesh& rhs);
    virtual ~CMesh() = default;
public:
    HRESULT                         Ready_Mesh(const wstring& pFilePath, const _tchar* pSaveFilePath);
private:
    void                            Load_MeshFromNode(FbxNode* pNode);
    void                            Load_Mesh(FbxMesh* pMesh);
    void                            Laod_Material(FbxSurfaceMaterial* pMaterial);
private:
    void                            Load_Texture();
public:
    HRESULT                         SetUp_Texture();
private:
    void                            Load_Skeleton(FbxNode* pNode);
    void                            Load_Skeleton_(FbxNode* pNode, _int iDepth, _int iIdx, _int iParentIdx);
private:
    void                            Load_AnimationClip();
    void                            Triangulate(FbxNode* pNode);
private:
    void                            GetTangent(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iVtxOrder);
    void                            GetNormal(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iVtxOrder);
    void                            GetBinormal(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iVtxOrder);
    void                            GetUV(FbxMesh* pMesh, tContainer* pContainer, _int iIdx, _int iUVIndex);
private:
    void                            Load_AnimationData(FbxMesh* pMesh, tContainer* pContainer);
    void                            LoadWeightsAndIndices(FbxCluster* _pCluster, int _iBoneIdx, tContainer* _pContainer);
    void                            LoadOffsetMatrix(FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform, int _iBoneIdx, tContainer* _pContainer);
    void                            LoadKeyframeTransform(FbxNode* _pNode, FbxCluster* _pCluster, const FbxAMatrix& _matNodeTransform
                                                        , int _iBoneIdx, tContainer* _pContainer);
    private:
    int                             FindBoneIndex(string _strBoneName);
    FbxAMatrix                      GetTransform(FbxNode* _pNode);
public:
    const   vector<tMTBone>*        GetBones() { return &m_vecMTBone; }
    vector<tMTAnimClip>             GetAnimClip() { return m_vecMTAnimClip; }
public:
    _matrix*                        Get_FindFrame(const _tchar* pFrameName);
public:
    const vector<tContainer>        GetContainer() { return m_vecContainer; }
private:
    _bool                           IsAnimation();
private:
    _matrix                         GetMatrix(FbxAMatrix& _mat)
    {
        _matrix mat;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                mat.m[i][j] = (float)_mat.Get(i, j);
            }
        }
        return mat;
    }
public:
    UINT                            GetBoneCount() { return (UINT)m_vecMTBone.size(); }
private:    
    void                            CheckWeightAndIndices(FbxMesh* _pMesh, tContainer* _pContainer);
private:
    _vec4                           GetMtrlData(FbxSurfaceMaterial* _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName);
    wstring                         GetMtrlTextureName(FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty);
private:
    HRESULT                         Ready_MeshData(tContainer* pContainer);
    HRESULT                         Ready_MeshData(vector<tContainer>&  vecContainer);
    HRESULT                         Save(const _tchar* pFilePath);
    HRESULT                         Load(const _tchar* pFilePath);
public:
    void                            Render_Mesh(_uint iIdx);
public:
    CStructedBuffer*                GetBoneFrameData(){return m_pBoneFrameData;}
    CStructedBuffer*                GetBoneOffset(){return m_pBoneOffset;}
private:
    RenderInfo                      m_tRenderInfo = {};
    vector< RenderInfo>             m_vecRenderInfo;
public:
    static CMesh*                   Create(const wstring& pFilePath, const _tchar* pSaveFilePath = nullptr);
    static CMesh*                   Create_Load(const _tchar* pFilePath);
    virtual CComponent*             Clone_Component(void* pArg);
private:
    FbxScene*                       m_pScene = nullptr;
private:
    vector<tBone*>					m_vecBone;
    vector<tAnimClip*>				m_vecAnimClip;
    vector<tContainer>              m_vecContainer;
    FbxArray<FbxString*>			m_arrAnimName;
public:
    vector<const _tchar*>           m_vecDiffTexturePath;
private:
    vector<tMTAnimClip>			    m_vecMTAnimClip;
    vector<tMTBone>                 m_vecMTBone;
private:
    _uint                           m_iSubsetNum = 0;
    _uint                           m_iCurTexNum = 0;
    _uint                           m_iMaxTexNum = 0;
private:
    CStructedBuffer*                m_pBoneFrameData; // 전체 본 프레임 정보
    CStructedBuffer*                m_pBoneOffset;	   // 각 뼈의 offset 행렬
public:
    const _uint&                    GetSubsetNum(){return m_iSubsetNum;}
private:
    virtual void                    Free();
};

