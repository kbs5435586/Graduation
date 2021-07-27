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
	, m_PrimitiveTopology(rhs.m_PrimitiveTopology)
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

void CVIBuffer::Render_VIBuffer(const _uint& iMaxParticleCnt)
{
	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(m_iSlot, 1, &m_tVertexBufferView);
	CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&m_tIndexBufferView);
	CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(m_iNumIndices, iMaxParticleCnt, 0, 0, 0);
}

void CVIBuffer::Render_VIBuffer(D3D12_PRIMITIVE_TOPOLOGY eTopology)
{
	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(eTopology);
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

	CComponent::Free();
}

_bool CVIBuffer::IntersectTriangle(_vec3& v0, _vec3& v1, _vec3& v2, _vec3 vPos, _vec3 vDir, _float* t, _float* u, _float* v)
{
	return _bool();
}