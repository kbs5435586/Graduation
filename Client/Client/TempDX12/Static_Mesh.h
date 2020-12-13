#pragma once
#include "Mesh.h"
class CHierachy_Loader;
class CStatic_Mesh :
    public CMesh
{
private:
	CStatic_Mesh(ID3D12Device* pGraphic_Device);
	CStatic_Mesh(const CStatic_Mesh& rhs);
	virtual ~CStatic_Mesh() = default;
public:
    HRESULT                     Ready_Static_Mesh(string strFilePath);
    FbxAMatrix                  ConvertMatrixToFbx(_matrix matWorld);
public:
    virtual void                Render_Mesh(ID3D12PipelineState* pPipeLine, CShader* pShader, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType);
    virtual void                Render_Mesh(ID3D12PipelineState* pPipeLine, CShader* pShader, _matrix matWorld, MAINPASS_LIGHT tPass, _int iPassSize, void* pData, ROOT_TYPE eType);
private:
    void                        Render_HierachyLoader(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxNode* pNode, _matrix matWorld,
                                                            _int iPassSize, void* pData, ROOT_TYPE eType);
    void                        Render_HierachyLoader(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxNode* pNode, _matrix matWorld,
                                                            MAINPASS_LIGHT tPass, _int iPassSize, void* pData, ROOT_TYPE eType);
private:
    void                        Render_Buffer(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxMesh* pMesh, FbxAMatrix& pFbxRootNodeMatrix,
                                                            FbxAMatrix& pGeomatryMatrix, _matrix matWorld, _int iPassSize, void* pData, ROOT_TYPE eType);
    void                        Render_Buffer(ID3D12PipelineState* pPipeLine, CShader* pShader, FbxMesh* pMesh, FbxAMatrix& pFbxRootNodeMatrix,
                                                            FbxAMatrix& pGeomatryMatrix, _matrix matWorld, MAINPASS_LIGHT tPass, _int iPassSize, void* pData, ROOT_TYPE eType);
    FbxAMatrix                  GetGeometricOffsetTransform(FbxNode* pNode);
public:
    CHierachy_Loader*           GetLoader() { return m_pLoader; }

public:
    static  CStatic_Mesh*       Create(ID3D12Device* pGraphic_Device, string strFilePath);
    virtual CComponent*         Clone_Component(void* pArg = nullptr);
protected:
    virtual void                Free();
private:
    CHierachy_Loader*           m_pLoader = nullptr;
};

