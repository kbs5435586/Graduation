#pragma once
#include "Base.h"
class CHierachy_Loader
	: public CBase
{
private:
	CHierachy_Loader(ID3D12Device* pGraphic_Device);
	~CHierachy_Loader() = default;
public:
	HRESULT								Ready_Hierachy_Loader(FbxNode* pFbxNode);
public:
	static CHierachy_Loader*			Create(ID3D12Device* pGraphic_Device, FbxScene* pFbxScene);
public:
	void								Render_Mesh(FbxScene* pFbxScene);
private:
	ID3D12Device*						m_pGraphic_Device = nullptr;
private:
	_int								m_iNumVertices = 0;
	_int								m_iNumIndices = 0;
	_int								m_iNumPolygons = 0;
private:
	vector<D3D12_VERTEX_BUFFER_VIEW>	m_vecVertexBufferView;
	vector<D3D12_INDEX_BUFFER_VIEW>		m_vecIndexBufferView;
private:
	vector<ID3D12Resource*>				m_vecVertexBuffer;
	vector<ID3D12Resource*>				m_vecVertexUploadBuffer;
	vector<ID3D12Resource*>				m_vecIndexBuffer;
	vector<ID3D12Resource*>				m_vecIndexUploadBuffer;
private:
	vector<_vec3>						m_vecPosition;
	vector<_uint>						m_vecIndecise;
private:
	virtual void						Free();
};