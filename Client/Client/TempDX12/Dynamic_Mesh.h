#pragma once
#include "Mesh.h"
class CHierachy_Loader;
class CAnimation_Controller;
class CShader;
class CDynamic_Mesh :
    public CMesh
{
private:
    CDynamic_Mesh(ID3D12Device* pGraphic_Device);
    CDynamic_Mesh(const CDynamic_Mesh& rhs);
    ~CDynamic_Mesh() = default;
public:
    HRESULT                     Ready_Dynamic_Mesh(string strFilePath);
    void                        Play_Animation(const _float& fTimeDelta);
    FbxAMatrix                  ConvertMatrixToFbx(_matrix matWorld);
public:
    virtual void                Render_Mesh(_matrix matWorld);
    void                        Render_Mesh(_matrix matWorld, ID3D12PipelineState* pPipeLine, ID3D12Resource* pConstantBuffer, CShader* pShader, void* pData, _int iIdx);
public:
    static  CDynamic_Mesh*      Create(ID3D12Device* pGraphic_Device, string strFilePath);
    virtual CComponent*         Clone_Component(void* pArg = nullptr);
public:
    void						AnimateFbxNodeHierarchy(FbxTime& fbxCurrentTime);
   
public:
    FbxTime                     Get_CurrentTime();
private:
    virtual void                Free();
private:
    CHierachy_Loader*           m_pLoader = nullptr;
    CAnimation_Controller*      m_pController = nullptr;
private:
    _float                      m_fTime = 0.f;
        

};

