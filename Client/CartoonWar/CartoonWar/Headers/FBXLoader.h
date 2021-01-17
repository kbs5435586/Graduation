#pragma once
#include "Base.h"
class CTexture;
class CFBXLoader :
    public CBase
{
private:
    CFBXLoader();
    virtual ~CFBXLoader() = default;
public:
    HRESULT                     Ready_FbxLoader(string strFilePath);
    HRESULT                     Load_FbxFile(FbxNode* pNode,_bool IsStatic = true);
private:
    HRESULT                     Load_Mesh(FbxMesh* pMesh, RenderInfo* pInfo);
    HRESULT                     Load_Material(FbxSurfaceMaterial* pMtrlSur, RenderInfo* pInfo);
public:
    HRESULT                     Load_Texture(RenderInfo* pInfo, CTexture* pTexture);
private:
    _vec4                       GetMtrlData(FbxSurfaceMaterial* pSurface, const char* pMtrlName, const char* pMtrlFactorName);
    wstring                     GetMtrlTextureName(FbxSurfaceMaterial* pSurface, const char* pMtrlProperty);
private:
    _vec3                       Get_Normal(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder);
    _vec2                       Get_UV(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder);
private:
    // Only VertexBuffer
    HRESULT                     CreateBufferView(RenderInfo* pInfo);
    // Use IndexBuffer
    HRESULT                     CreateBufferView_Index(RenderInfo* pInfo);
public:
    static CFBXLoader*          Create(string strFilePath);
private:
    virtual void                Free();
public:
    FbxScene*                   GetScene() { return m_pScene; }
    vector<RenderInfo*>         GetRenderInfo() { return m_vecRenderInfo; }
private:
    FbxScene*                   m_pScene = nullptr;
private:
    vector<RenderInfo*>		    m_vecRenderInfo;
private:
    _uint                       m_iNumVertices = 0;
    _uint                       m_iNumIndices = 0;
    _uint                       m_iStride = 0;
private:
    vector<ID3D12Resource*>     m_vecVertexBuffer;
    vector<ID3D12Resource*>     m_vecVertexUploadBuffer;
    vector<ID3D12Resource*>     m_vecIndexBuffer;
    vector<ID3D12Resource*>     m_vecIndexUploadBuffer;



};

