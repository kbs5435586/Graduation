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

void CVIBuffer::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(m_iSlot, 1, &m_tVertexBufferView);
	CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&m_tIndexBufferView);
	CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(m_iNumIndices, 1, 0, 0, 0);
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