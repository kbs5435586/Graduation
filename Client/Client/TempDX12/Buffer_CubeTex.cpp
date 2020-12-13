#include "framework.h"
#include "Buffer_CubeTex.h"

CBuffer_CubeTex::CBuffer_CubeTex(ID3D12Device* pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_CubeTex::CBuffer_CubeTex(const CBuffer_CubeTex& rhs)
	: CVIBuffer(rhs)
	, m_vecVertices(rhs.m_vecVertices)
	, m_vecIndices(rhs.m_vecIndices)
{
}

HRESULT CBuffer_CubeTex::Ready_VIBuffer()
{
	m_iVertices = 8;
	m_iStride = sizeof(VTXTEXCUBE);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vecVertices.resize(8);

	m_vecVertices[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	m_vecVertices[0].vTex = m_vecVertices[0].vPosition;
	m_vecVertices[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	m_vecVertices[1].vTex = m_vecVertices[1].vPosition;
	m_vecVertices[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	m_vecVertices[2].vTex = m_vecVertices[2].vPosition;
	m_vecVertices[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	m_vecVertices[3].vTex = m_vecVertices[3].vPosition;
	m_vecVertices[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	m_vecVertices[4].vTex = m_vecVertices[4].vPosition;
	m_vecVertices[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	m_vecVertices[5].vTex = m_vecVertices[5].vPosition;
	m_vecVertices[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	m_vecVertices[6].vTex = m_vecVertices[6].vPosition;
	m_vecVertices[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	m_vecVertices[7].vTex = m_vecVertices[7].vPosition;

	m_iIndices = 36;


	m_vecIndices.resize(m_iIndices);
	m_vecIndices[0] = 1; m_vecIndices[1] = 5; m_vecIndices[2] = 6;
	m_vecIndices[3] = 1; m_vecIndices[4] = 6; m_vecIndices[5] = 2;

	m_vecIndices[6] = 4; m_vecIndices[7] = 0; m_vecIndices[8] = 3;
	m_vecIndices[9] = 4; m_vecIndices[10] = 3; m_vecIndices[11] = 7;

	m_vecIndices[12] = 4; m_vecIndices[13] = 5; m_vecIndices[14] = 1;
	m_vecIndices[15] = 4; m_vecIndices[16] = 1; m_vecIndices[17] = 0;

	m_vecIndices[18] = 3; m_vecIndices[19] = 2; m_vecIndices[20] = 6;
	m_vecIndices[21] = 3; m_vecIndices[22] = 6; m_vecIndices[23] = 7;

	m_vecIndices[24] = 7; m_vecIndices[25] = 6; m_vecIndices[26] = 5;
	m_vecIndices[27] = 7; m_vecIndices[28] = 5; m_vecIndices[29] = 4;

	m_vecIndices[30] = 0; m_vecIndices[31] = 1; m_vecIndices[32] = 2;
	m_vecIndices[33] = 0; m_vecIndices[34] = 2; m_vecIndices[35] = 3;

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iVertices);

		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(m_vecVertices.data());
		vertexData.RowPitch = m_iStride * m_iVertices;
		vertexData.SlicePitch = m_iStride * m_iVertices;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCommandList(), m_pVertexBuffer, m_pVertexUploadBuffer, 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCommandList()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iIndices);


		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		m_pIndexBuffer->SetName(L"IndexBuffer");
		if (FAILED(m_pGraphic_Device->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, &tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;
		m_pIndexUploadBuffer->SetName(L"Upload IndexBuffer");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(m_vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * m_iIndices;
		indexData.SlicePitch = sizeof(_uint) * m_iIndices;

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

void CBuffer_CubeTex::Render_VIBuffer()
{
	CDevice::GetInstance()->GetCommandList()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCommandList()->IASetVertexBuffers(m_iSlot, 1, &m_VertexBufferView);
	CDevice::GetInstance()->GetCommandList()->IASetIndexBuffer(&m_IndexBufferView);
	CDevice::GetInstance()->GetCommandList()->DrawIndexedInstanced(m_iIndices, 1, 0, 0, 0);
}

CBuffer_CubeTex* CBuffer_CubeTex::Create(ID3D12Device* pGraphic_Device)
{
	CBuffer_CubeTex* pInstance = new CBuffer_CubeTex(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_CubeTex Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_CubeTex::Clone_Component(void* pArg)
{
	return new CBuffer_CubeTex(*this);
}

void CBuffer_CubeTex::Free()
{
	CVIBuffer::Free();
}
