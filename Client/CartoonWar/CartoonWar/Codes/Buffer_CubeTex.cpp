#include "framework.h"
#include "Buffer_CubeTex.h"

CBuffer_CubeTex::CBuffer_CubeTex()
	: CVIBuffer()
{
}

CBuffer_CubeTex::CBuffer_CubeTex(const CBuffer_CubeTex& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_CubeTex::Ready_VIBuffer()
{
	m_iNumVertices = 8;
	m_iStride = sizeof(VTXTEXCUBE);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vector< VTXTEXCUBE> vecVertices;
	vecVertices.resize(8);

	vecVertices[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	vecVertices[0].vTex = vecVertices[0].vPosition;

	vecVertices[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	vecVertices[1].vTex = vecVertices[1].vPosition;

	vecVertices[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	vecVertices[2].vTex = vecVertices[2].vPosition;

	vecVertices[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	vecVertices[3].vTex = vecVertices[3].vPosition;

	vecVertices[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	vecVertices[4].vTex = vecVertices[4].vPosition;

	vecVertices[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	vecVertices[5].vTex = vecVertices[5].vPosition;

	vecVertices[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	vecVertices[6].vTex = vecVertices[6].vPosition;

	vecVertices[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	vecVertices[7].vTex = vecVertices[7].vPosition;

	m_iNumIndices = 36;

	vector<_uint>	vecIndices;
	vecIndices.resize(m_iNumIndices);
	vecIndices[0] = 1; vecIndices[1] = 5; vecIndices[2] = 6;
	vecIndices[3] = 1; vecIndices[4] = 6; vecIndices[5] = 2;

	vecIndices[6] = 4; vecIndices[7] = 0; vecIndices[8] = 3;
	vecIndices[9] = 4; vecIndices[10] = 3; vecIndices[11] = 7;

	vecIndices[12] = 4; vecIndices[13] = 5; vecIndices[14] = 1;
	vecIndices[15] = 4; vecIndices[16] = 1; vecIndices[17] = 0;

	vecIndices[18] = 3; vecIndices[19] = 2; vecIndices[20] = 6;
	vecIndices[21] = 3; vecIndices[22] = 6; vecIndices[23] = 7;

	vecIndices[24] = 7; vecIndices[25] = 6; vecIndices[26] = 5;
	vecIndices[27] = 7; vecIndices[28] = 5; vecIndices[29] = 4;

	vecIndices[30] = 0; vecIndices[31] = 1; vecIndices[32] = 2;
	vecIndices[33] = 0; vecIndices[34] = 2; vecIndices[35] = 3;

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
		vertexData.RowPitch = m_iStride * m_iNumVertices;
		vertexData.SlicePitch = m_iStride * m_iNumVertices;

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

CBuffer_CubeTex* CBuffer_CubeTex::Create()
{
	CBuffer_CubeTex* pInstance = new CBuffer_CubeTex();

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
