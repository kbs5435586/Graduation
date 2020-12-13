#pragma once
#include "Base.h"
class CTexture;
class CHierachy_Loader :
    public CBase
{
private:
	CHierachy_Loader(ID3D12Device* pGraphic_Device);
	~CHierachy_Loader() = default;
public:
	HRESULT								Ready_Hierachy_Loader(string strFilePath, FbxScene*& pScene);
public:
	HRESULT								Ready_Load_Hierachy(FbxNode* pNode, _bool IsStatic = true);

private:
	HRESULT								CreateBufferView(_uint iVerticesNum, RenderInfo* pInfo);
	HRESULT								CreateBufferView_Dynamic(_uint iVerticesNum,  RenderInfo* pInfo);
	HRESULT								CreateBufferView__(_uint iVerticesNum, _uint iIndicesNum, RenderInfo* pInfo);
public:
	vector<RenderInfo*>					GetRenderInfo() { return m_vecRenderInfo; }
public:
	static CHierachy_Loader*			Create(ID3D12Device* pGraphic_Device,string strFilePath,  FbxScene*& pScene);
	virtual void						Free();
private:
	HRESULT								GetTangent(FbxMesh* pMesh, RenderInfo* pInfo, _uint iIdx, _uint iVtxOrder);
	HRESULT								GetBiNormal(FbxMesh* pMesh, RenderInfo* pInfo, _uint iIdx, _uint iVtxOrder);
	_vec3								GetNormal(FbxMesh* pMesh,_uint iIdx, _uint iVtxOrder);
	_vec2								GetUV(FbxMesh* pMesh, _uint iIdx, _uint iVtxOrder);
private:
	_vec4								GetMtrlData(FbxSurfaceMaterial* pSurface, const char* pMtrlName, const char* pMtrlFactorName);
	wstring								GetMtrlTextureName(FbxSurfaceMaterial* pSurface, const char* pMtrlProperty);
private:
	HRESULT								LoadMesh(FbxMesh* pMesh, RenderInfo* pInfo);
	HRESULT								LoadDynamicMesh(FbxMesh* pMesh, RenderInfo* pInfo);
	HRESULT								LoadMeshIndexBuffer(FbxMesh* pMesh, RenderInfo* pInfo);
	HRESULT								LoadMeshIndexBuffer_(FbxMesh* pMesh, RenderInfo* pInfo);
private:
	HRESULT								LoadMaterial(FbxSurfaceMaterial* pMtrlSur, RenderInfo* pInfo);
	HRESULT								LoadTexture(RenderInfo* pInfo);


public:
	CTexture*							GetTexture(_uint iTextureIdx) { return m_vecTexture[iTextureIdx]; }
private:
	ID3D12Device*						m_pGraphic_Device = nullptr;
private:
	vector<RenderInfo*>					m_vecRenderInfo;
private:
	vector<ID3D12Resource*>				m_vecVertexBuffer;
	vector<ID3D12Resource*>				m_vecVertexUploadBuffer;
	vector<ID3D12Resource*>				m_vecIndexBuffer;
	vector<ID3D12Resource*>				m_vecIndexUploadBuffer;
private:
	D3D12_PRIMITIVE_TOPOLOGY			m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_uint								m_iSlot = 0;
	_uint								m_iVertices = 0;
	_uint								m_iStride = 0;
	_uint								m_iOffset = 0;
private:
	_uint								m_iIndices = 0;
private:
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView;
	D3D12_INDEX_BUFFER_VIEW				m_IndexBufferView;
private:
	vector<CTexture*>					m_vecTexture;

};

