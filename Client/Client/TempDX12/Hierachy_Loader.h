#pragma once
#include "Base.h"
class CHierachy_Loader :
    public CBase
{
private:
	CHierachy_Loader(ID3D12Device* pGraphic_Device);
	~CHierachy_Loader() = default;
public:
	HRESULT								Ready_Hierachy_Loader(string strFilePath, FbxScene*& pScene);
public:
	HRESULT								Ready_Load_Hierachy(FbxNode* pNode);
	HRESULT								CreateBufferView(_uint iVerticesNum, _uint iIndicesNum, vector<_vec3>& vecPos, RenderInfo* pInfo);
	HRESULT								CreateBuffer(_uint iVerticesNum, _uint iIndicesNum, RenderInfo* pInfo);
public:
	vector<RenderInfo*>					GetRenderInfo() { return m_vecRenderInfo; }
public:
	static CHierachy_Loader*			Create(ID3D12Device* pGraphic_Device,string strFilePath,  FbxScene*& pScene);
	virtual void						Free();
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
};

