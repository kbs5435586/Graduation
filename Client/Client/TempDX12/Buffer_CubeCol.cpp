#include "framework.h"
#include "Buffer_CubeCol.h"

CBuffer_CubeCol::CBuffer_CubeCol(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_CubeCol::CBuffer_CubeCol(const CBuffer_CubeCol& rhs)
	: CVIBuffer(rhs)
	, m_vecVertex(rhs.m_vecVertex)
{
}

HRESULT CBuffer_CubeCol::Ready_VIBuffer()
{
	m_iVertices = 8;
	m_iStride = sizeof(VTXCOL);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vecVertex.resize(8);
	VTXCOL pVertices[8];
	pVertices[0] = VTXCOL(XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	pVertices[1] = VTXCOL(XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.f));
	pVertices[2] = VTXCOL(XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
	pVertices[3] = VTXCOL(XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
	pVertices[4] = VTXCOL(XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.f));
	pVertices[5] = VTXCOL(XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.f));
	pVertices[6] = VTXCOL(XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.f));
	pVertices[7] = VTXCOL(XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.f));


	for (_int i = 0; i < 8; ++i)
	{
		m_vecVertex[i] = pVertices[i];
	}
	m_iIndices = 36;

	_uint	indicies[36]{};
	indicies[0] = 1; indicies[1] = 5; indicies[2] = 6;
	indicies[3] = 1; indicies[4] = 6; indicies[5] = 2;

	indicies[6] = 4; indicies[7] = 0; indicies[8] = 3;
	indicies[9] = 4; indicies[10] = 3; indicies[11] = 7;

	indicies[12] = 4; indicies[13] = 5; indicies[14] = 1;
	indicies[15] = 4; indicies[16] = 1; indicies[17] = 0;

	indicies[18] = 3; indicies[19] = 2; indicies[20] = 6;
	indicies[21] = 3; indicies[22] = 6; indicies[23] = 7;

	indicies[24] = 7; indicies[25] = 6; indicies[26] = 5;
	indicies[27] = 7; indicies[28] = 5; indicies[29] = 4;

	indicies[30] = 0; indicies[31] = 1; indicies[32] = 2;
	indicies[33] = 0; indicies[34] = 2; indicies[35] = 3;

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);
	
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default,D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,IID_PPV_ARGS(&m_pVertexBuffer))))
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
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices);
	

		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default,D3D12_HEAP_FLAG_NONE,&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(indicies);
		indexData.RowPitch = sizeof(indicies); 
		indexData.SlicePitch = sizeof(indicies);

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
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

void CBuffer_CubeCol::Render_VIBuffer()
{
	for (auto& iter : m_vecVertex)
	{
		iter.vPosition.x+=1.f;
	}


	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_iIndices, 1, 0, 0, 0);
}

CBuffer_CubeCol* CBuffer_CubeCol::Create(ID3D12Device* pGraphic_Device)
{
	CBuffer_CubeCol* pInstance = new CBuffer_CubeCol(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_CubeCol Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_CubeCol::Clone_Component(void* pArg)
{
	return new CBuffer_CubeCol(*this);
}

void CBuffer_CubeCol::Free()
{
	CVIBuffer::Free();
}
