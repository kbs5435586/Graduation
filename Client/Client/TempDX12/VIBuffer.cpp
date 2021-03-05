#include "framework.h"
#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVertexBuffer(rhs.m_pVertexBuffer)
	, m_pVertexUploadBuffer(rhs.m_pVertexUploadBuffer)
	, m_pIndexBuffer(rhs.m_pIndexBuffer)
	, m_pIndexUploadBuffer(rhs.m_pIndexUploadBuffer)
	, m_PrimitiveTopology(rhs.m_PrimitiveTopology)
	, m_iSlot(rhs.m_iSlot)
	, m_iVertices(rhs.m_iVertices)
	, m_iStride(rhs.m_iStride)
	, m_iOffset(rhs.m_iOffset)
	, m_iIndices(rhs.m_iIndices)
	, m_iStartIndex(rhs.m_iStartIndex)
	, m_iBaseVertex(rhs.m_iBaseVertex)
	, m_VertexBufferView(rhs.m_VertexBufferView)
	, m_IndexBufferView(rhs.m_IndexBufferView)
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->AddRef();
	if (m_pVertexUploadBuffer)
		m_pVertexUploadBuffer->AddRef();
	if (m_pIndexBuffer)
		m_pIndexBuffer->AddRef();
	if (m_pIndexUploadBuffer)
		m_pIndexUploadBuffer->AddRef();
}

HRESULT CVIBuffer::Ready_VIBuffer()
{
	return S_OK;
}

CComponent* CVIBuffer::Clone_Component(void* pArg)
{
	return nullptr;
}

void CVIBuffer::Free()
{
	if (m_pIndexBuffer)
		Safe_Release(m_pIndexBuffer);
	if (m_pIndexUploadBuffer)
		Safe_Release(m_pIndexUploadBuffer);
	if (m_pVertexBuffer)
		Safe_Release(m_pVertexBuffer);
	if (m_pVertexUploadBuffer)
		Safe_Release(m_pVertexUploadBuffer);


	CComponent::Free();
}
