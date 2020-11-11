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
public:
    void                            AnimateMesh(FbxMesh* pMesh);
public:
    void                            ComputeSkinDeformation(FbxMesh* pMesh, FbxTime& fbxCurTime, 
                                    FbxVector4* pVertices, _int iVertices);
private:

public:
    FbxTime                         GetCurrentTime() { return m_pFbxCurrTime[m_iCurrAnimationStack]; }
private:
    _int                            m_iAnimationStack = 0;
    _int                            m_iCurrAnimationStack = 0;
private:
    FbxAnimStack**                  m_pFbxAnimStack = nullptr;
    vector<FbxAnimStack*>           m_vecFbxAnimStack;
    FbxTime*                        m_pFbxStartTime = nullptr;
    FbxTime*                        m_pFbxStopTime = nullptr;
    FbxTime*                        m_pFbxCurrTime = nullptr;
};

