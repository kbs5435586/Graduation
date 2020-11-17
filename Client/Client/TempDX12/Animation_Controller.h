#pragma once
#include "Base.h"
class CAnimation_Controller :
    public CBase
{
private:
    CAnimation_Controller(ID3D12Device* pGraphic_Device);
    ~CAnimation_Controller() = default;
public:
    HRESULT                         Ready_Animation_Controller(FbxScene* pScene);
public:
    static CAnimation_Controller*   Create(ID3D12Device* pGraphic_Device, FbxScene* pScene);
private:
    virtual void				    Free();
public:
    void                            SetPosition(_int iAnimStack, _float fPosition);
    void                            SetAnimationStack(FbxScene* pScene, _int iAnimStack);
    void                            AdvacneTime(const _float& fTimeDelta);
    FbxTime                         GetCurrTime() { return m_pFbxCurrTime[m_iCurrAnimationStack]; }
public:
    void                            AnimateHierachyMesh(FbxNode* pNode, FbxTime& fbxCurrTime);
    void                            AnimateMesh(FbxMesh* pMesh, FbxTime& fbxCurrTime);
private:
    FbxAMatrix                      ComputeClusterDeformation(FbxMesh* pMesh, FbxCluster* pCluster, FbxCluster::ELinkMode iClusterMode, FbxTime& fbxCurrTime);
    void                            ComputeSkinDeformation(FbxMesh* pMesh, FbxTime& fbxCurrTime, FbxVector4* pVertices, _int iNumVertices);
    void                            ComputeLinearDeformation(FbxMesh* pMesh, FbxTime& fbxCurrTime, FbxVector4* pVertices, _int iNumVertices);
    void                            ComputeDualQuaternionDeformation(FbxMesh* pMesh, FbxTime& fbxCurrTime, FbxVector4* pVertices, _int iNumVertices);
private:
    void                            MatrixScale(FbxAMatrix& fbxmatSrcMatrix, double pValue);
    void                            MatrixAdd(FbxAMatrix& fbxmatDstMatrix, FbxAMatrix& fbxmatSrcMatrix);
    void                            MatrixAddToDiagonal(FbxAMatrix& fbxmatSrcMatrix, double pValue);
    FbxAMatrix                      GetGeomatryOffsetTransform(FbxNode* pNode);
private:
    HRESULT						    CreateBufferView(_uint iVerticesNum, _uint iIndicesNum, vector<_vec3>& vecPos, RenderInfo* pInfo);

public:
    FbxTime                         GetCurrentTime() { return m_pFbxCurrTime[m_iCurrAnimationStack]; }
private:
    _int                            m_iAnimationStack = 0;
    _int                            m_iCurrAnimationStack = 0;
private:
    FbxAnimStack**                  m_pFbxAnimStack = nullptr;
    FbxTime*                        m_pFbxStartTime = nullptr;
    FbxTime*                        m_pFbxStopTime = nullptr;
    FbxTime*                        m_pFbxCurrTime = nullptr;
};

