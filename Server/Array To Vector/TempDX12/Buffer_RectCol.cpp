#include "framework.h"
#include "Buffer_RectCol.h"

CBuffer_RectCol::CBuffer_RectCol(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_RectCol::CBuffer_RectCol(const CBuffer_RectCol& rhs)
	: CVIBuffer(rhs)
	, m_vecVertices(rhs.m_vecVertices)
	, m_vecIndices(rhs.m_vecIndices)
{
}

HRESULT CBuffer_RectCol::Ready_VIBuffer()
{
	m_iVertices = 4;
	m_iStride = sizeof(VTXCOL);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	m_vecVertices.resize(4);

	m_vecVertices[0] = VTXCOL(XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	m_vecVertices[1] = VTXCOL(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	m_vecVertices[2] = VTXCOL(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	m_vecVertices[3] = VTXCOL(XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));

	m_iIndices = 6;


	m_vecIndices.resize(m_iIndices);
	m_vecIndices[0]=0;
	m_vecIndices[1]=1;
	m_vecIndices[2]=2;
	m_vecIndices[3]=0;
	m_vecIndices[4]=3;
	m_vecIndices[5]=2;

	CDevice::GetInstance()->Open();
	{
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_iStride*m_iVertices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(m_vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iVertices;
		vertexData.SlicePitch = m_iStride * m_iVertices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pVertexBuffer, m_pVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}

	{
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint)*m_iIndices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(m_vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pIndexBuffer, m_pIndexUploadBuffer, 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
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

void CBuffer_RectCol::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_iIndices, 1, 0, 0, 0);
}

CBuffer_RectCol* CBuffer_RectCol::Create(ID3D12Device* pGraphic_Device)
{
	CBuffer_RectCol* pInstance = new CBuffer_RectCol(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_RectCol Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CBuffer_RectCol::Clone_Component(void* pArg)
{
	return new CBuffer_RectCol(*this);
}
void CBuffer_RectCol::Free()
{
	CVIBuffer::Free();
}

