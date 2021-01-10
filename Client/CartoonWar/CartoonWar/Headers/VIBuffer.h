#pragma once
#include "Component.h"
class CVIBuffer :
    public CComponent
{
protected:
	CVIBuffer();
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;
public:
	HRESULT						Ready_VIBuffer();
public:
	virtual CComponent*			Clone_Component(void* pArg = nullptr) = 0;
protected:
	virtual void				Free();
protected:
	ID3D12Resource*				m_pVertexBuffer = nullptr;
	ID3D12Resource*				m_pVertexUploadBuffer = nullptr;
	ID3D12Resource*				m_pIndexBuffer = nullptr;
	ID3D12Resource*				m_pIndexUploadBuffer = nullptr;
protected:
	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_uint						m_iSlot = 0;
	_uint						m_iStride = 0;
	_uint						m_iOffset = 0;
protected:
	_uint						m_iNumVertices = 0;
	_uint						m_iNumIndices = 0;
protected:
	D3D12_VERTEX_BUFFER_VIEW	m_tVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_tIndexBufferView;


};

