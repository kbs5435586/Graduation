#pragma once
#include "Component.h"
class CFBXLoader;
class CShader;
class CTexture;
class CStatic_Mesh :
    public CComponent
{
private:
    CStatic_Mesh();
    CStatic_Mesh(const CStatic_Mesh& rhs);
    virtual ~CStatic_Mesh() = default;
public:
    HRESULT                 Ready_Static_Mesh(string strFilePath);
    void                    Render_Hierachy_Mesh(FbxNode* pNode, CShader* pShaderCom, _matrix matWorld, MAINPASS& tPass, 
                                                CTexture* pTexture0, CTexture* pTexture1, CTexture* pTexture2);

    void                    Render_Mesh(CShader* pShaderCom, FbxMesh* pMesh, FbxAMatrix& pRootNodeMatrix,
                                        FbxAMatrix& pGeometryMatrix, _matrix matWorld, MAINPASS& tPass, CTexture* pTexture0, CTexture* pTexture1, CTexture* pTexture2);
private:
    FbxAMatrix              GetGeometricOffsetTransform(FbxNode* pNode);
    FbxAMatrix              ConvertMatrixToFbx(_matrix matWorld);
    _matrix                 FbxMatrixToMatrix(FbxAMatrix* pFbxMatrix);
public:
    static CStatic_Mesh*    Create(string strFilePath);
    virtual CComponent*     Clone_Component(void* pArg);
private:
    virtual void            Free();
public:
    CFBXLoader*             GetLoader() { return m_pLoader; }
private:
    CFBXLoader*             m_pLoader = nullptr;
private:
    _int                    m_iTestNum = 0;

};

