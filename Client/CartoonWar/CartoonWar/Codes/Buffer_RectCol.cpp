#include "framework.h"
#include "Buffer_RectCol.h"

CBuffer_RectCol::CBuffer_RectCol()
	: CVIBuffer()
{
}

CBuffer_RectCol::CBuffer_RectCol(const CBuffer_RectCol& rhs)
	: CVIBuffer(rhs)

{
}

HRESULT CBuffer_RectCol::Ready_VIBuffer()
{
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXCOL);

	vector<VTXCOL>	vecVertices;
	vecVertices.resize(m_iNumVertices);

	vecVertices[0] = VTXCOL(XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	vecVertices[1] = VTXCOL(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	vecVertices[2] = VTXCOL(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	vecVertices[3] = VTXCOL(XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));

	m_iNumIndices = 6;
	vector<_uint>	vecIndices;
	vecIndices.resize(m_iNumIndices);

	vecIndices[0] = 0;
	vecIndices[1] = 2;
	vecIndices[2] = 1;
	vecIndices[3] = 0;
	vecIndices[4] = 3;
	vecIndices[5] = 2;

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumIndices);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iNumIndices;
		vertexData.SlicePitch = m_iStride * m_iNumIndices;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer, m_pVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices);


		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iNumIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer, m_pIndexUploadBuffer, 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();


	m_tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_tVertexBufferView.StrideInBytes = m_iStride;
	m_tVertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

	m_tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_tIndexBufferView.SizeInBytes = sizeof(_uint) * m_iNumIndices;

	CDevice::GetInstance()->WaitForFenceEvent();
	return S_OK;
}


CBuffer_RectCol* CBuffer_RectCol::Create()
{
	CBuffer_RectCol* pInstance = new CBuffer_RectCol();

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
