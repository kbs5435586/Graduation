#include "framework.h"
#include "VIBuffer.h"

CVIBuffer::CVIBuffer()
	: CComponent()
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_tVertexBufferView(rhs.m_tVertexBufferView)
	, m_tIndexBufferView(rhs.m_tIndexBufferView)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_iNumVertices(rhs.m_iNumVertices)
{

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
