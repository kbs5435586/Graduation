#include "framework.h"
#include "Buffer_RectTex.h"

CBuffer_RectTex::CBuffer_RectTex(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_RectTex::CBuffer_RectTex(const CBuffer_RectTex& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CBuffer_RectTex::Ready_VIBuffer()
{
	m_iVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	VTXTEX pVertices[4];
	pVertices[0] = VTXTEX(XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT2(0.0f, 0.0f));
	pVertices[1] = VTXTEX(XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT2(1.0f, 0.0f));
	pVertices[2] = VTXTEX(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT2(1.f, 1.f));
	pVertices[3] = VTXTEX(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT2(0.0f, 1.f));


	m_iIndices = 6;

	_uint	indicies[6]{};
	indicies[0] = 0; indicies[1] = 1; indicies[2] = 2;
	indicies[3] = 0; indicies[4] = 2; indicies[5] = 3;

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);
	


		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(pVertices);
		vertexData.RowPitch = sizeof(pVertices);
		vertexData.SlicePitch = sizeof(pVertices);

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pVertexBuffer, m_pVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indicies) * m_iIndices);
	

		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(indicies);
		indexData.RowPitch = sizeof(indicies);
		indexData.SlicePitch = sizeof(indicies);

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pIndexBuffer, m_pIndexUploadBuffer, 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();

	m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = m_iStride;
	m_VertexBufferView.SizeInBytes = m_iStride * m_iVertices;

	m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_IndexBufferView.SizeInBytes = sizeof(_uint) * m_iIndices;

	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}

void CBuffer_RectTex::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_iIndices, 1, 0, 0, 0);
}

CBuffer_RectTex* CBuffer_RectTex::Create(ID3D12Device* pGraphic_Device)
{
	CBuffer_RectTex* pInstance = new CBuffer_RectTex(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_RectTex Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_RectTex::Clone_Component(void* pArg)
{
	return new CBuffer_RectTex(*this);
}

void CBuffer_RectTex::Free()
{
	CVIBuffer::Free();
}
