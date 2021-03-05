#pragma once
#include "Component.h"
class CTexture;
class CMesh :
    public CComponent
{
private:
    CMesh();
    CMesh(const CMesh& rhs);
    virtual ~CMesh() = default;
public:
    HRESULT                         Ready_Mesh(const wstring& pFilePath);
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
private:
    void                            CheckWeightAndIndices(FbxMesh* _pMesh, tContainer* _pContainer);
private:
    _vec4                           GetMtrlData(FbxSurfaceMaterial* _pSurface, const char* _pMtrlName, const char* _pMtrlFactorName);
    wstring                         GetMtrlTextureName(FbxSurfaceMaterial* _pSurface, const char* _pMtrlProperty);
private:
    HRESULT                         Ready_MeshData(tContainer* pContainer);
public:
    void                            Render_Mesh(_uint iIdx);
private:
    RenderInfo                      m_tRenderInfo = {};
public:
    static CMesh*                   Create(const wstring& pFilePath);
    virtual CComponent*             Clone_Component(void* pArg);
private:
    FbxScene*                       m_pScene = nullptr;
private:
    vector<tBone*>					m_vecBone;
    vector<tAnimClip*>				m_vecAnimClip;
    vector<tContainer>              m_vecContainer;
    FbxArray<FbxString*>			m_arrAnimName;
private:
    vector<CTexture*>               m_vecTexture;
private:
    _uint                           m_iSubsetNum = 0;
    _uint                           m_iCurTexNum = 0;
    _uint                           m_iMaxTexNum = 0;
public:
    const _uint&                    GetSubsetNum(){return m_iSubsetNum;}
    vector<CTexture*>               GetTexture() { return m_vecTexture; }
private:
    virtual void        Free();
};

