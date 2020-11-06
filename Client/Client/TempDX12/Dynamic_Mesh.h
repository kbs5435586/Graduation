#pragma once
#include "Mesh.h"
class CLoader_Test;
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
    FbxAMatrix                  ConvertMatrixToFbx(_matrix matWorld);
private:
    void                        Render_HierachyLoader(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxNode* pNode, _matrix matWorld, 
                                                      _int iPassSize, void* pData, ROOT_TYPE eType);
    void                        Render_Buffer(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxMesh* pMesh, FbxAMatrix& pFbxRootNodeMatrix,
                                                FbxAMatrix& pGeomatryMatrix, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType);
    FbxAMatrix                  GetGeometricOffsetTransform(FbxNode* pNode);
public:
    virtual void                Render_Mesh(ID3D12PipelineState* pPipeLine, CShader* pShader, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType);
public:
    static  CDynamic_Mesh*      Create(ID3D12Device* pGraphic_Device, string strFilePath);
    virtual CComponent*         Clone_Component(void* pArg = nullptr);
private:
    virtual void                Free();
private:
    CLoader_Test*               m_pLoader = nullptr;


        

};

