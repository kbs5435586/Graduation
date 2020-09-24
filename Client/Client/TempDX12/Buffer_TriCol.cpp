#include "framework.h"
#include "Buffer_TriCol.h"


CBuffer_TriCol::CBuffer_TriCol(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_TriCol::CBuffer_TriCol(const CBuffer_TriCol& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CBuffer_TriCol::Ready_VIBuffer()
{
	m_iVertices = 3;
	m_iStride = sizeof(VTXCOL);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VTXCOL pVertices[3];
	pVertices[0] = VTXCOL(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	pVertices[1] = VTXCOL(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.f));
	pVertices[2] = VTXCOL(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));


	if (FAILED(m_pGraphic_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(pVertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVertexBuffer))))
		return E_FAIL;

	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0); 
	if(FAILED(m_pVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin))))
		return E_FAIL;
	memcpy(pVertexDataBegin, pVertices, sizeof(pVertices));
	m_pVertexBuffer->Unmap(0, nullptr);


	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = m_iStride;
	m_VertexBufferView.SizeInBytes = m_iStride * m_iVertices;

	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}

void CBuffer_TriCol::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawInstanced(m_iVertices, 1, m_iOffset, 0);
}

CBuffer_TriCol* CBuffer_TriCol::Create(ID3D12Device* pGraphic_Device)
{
	CBuffer_TriCol* pInstance = new CBuffer_TriCol(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_TriCol Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_TriCol::Clone_Component(void* pArg)
{
	return new CBuffer_TriCol(*this);
}

void CBuffer_TriCol::Free()
{
	CVIBuffer::Free();
}
