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
public:
    static  CDynamic_Mesh*      Create(ID3D12Device* pGraphic_Device, string strFilePath);
    virtual CComponent*         Clone_Component(void* pArg = nullptr);
private:
    virtual void                Free();
private:
    CHierachy_Loader*           m_pLoader = nullptr;
    CAnimation_Controller*      m_pController = nullptr;
private:
    _float                      m_fTime = 0.f;
        

};

