#include "framework.h"
#include "Buffer_RectTexNor.h"

CBuffer_RectTexNor::CBuffer_RectTexNor()
	: CVIBuffer()
{
}

CBuffer_RectTexNor::CBuffer_RectTexNor(const CBuffer_RectTexNor& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_RectTexNor::Ready_VIBuffer()
{
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEXBUMP);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vector<VTXTEXBUMP>		vecVertices;
	vecVertices.resize(m_iNumVertices);
	vecVertices[0] = VTXTEXBUMP(XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f));
	vecVertices[1] = VTXTEXBUMP(XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f));
	vecVertices[2] = VTXTEXBUMP(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT2(1.f, 1.f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f));
	vecVertices[3] = VTXTEXBUMP(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT3(0.f, 0.f, -1.f), XMFLOAT2(0.0f, 1.f), XMFLOAT3(0.f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f));

	m_iNumIndices = 6;

	vector<_uint>	vecIndices;
	vecIndices.resize(m_iNumIndices);
	vecIndices[0] = 0; vecIndices[1] = 1; vecIndices[2] = 2;
	vecIndices[3] = 0; vecIndices[4] = 2; vecIndices[5] = 3;

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);



		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iNumIndices;;
		vertexData.SlicePitch = m_iStride * m_iNumIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices);


		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iNumIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;

		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
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



CBuffer_RectTexNor* CBuffer_RectTexNor::Create()
{
	CBuffer_RectTexNor* pInstance = new CBuffer_RectTexNor();

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_RectTexNor Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_RectTexNor::Clone_Component(void* pArg)
{
	return new CBuffer_RectTexNor(*this);
}

void CBuffer_RectTexNor::Free()
{
	CVIBuffer::Free();
}
