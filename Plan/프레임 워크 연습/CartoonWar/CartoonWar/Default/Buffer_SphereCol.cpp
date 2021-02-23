#include "framework.h"
#include "Buffer_SphereCol.h"
#include <cmath>
#define _USE_MATH_DEFINES


CBuffer_SphereCol::CBuffer_SphereCol()
	: CVIBuffer()
{
}

CBuffer_SphereCol::CBuffer_SphereCol(const CBuffer_SphereCol& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_SphereCol::Ready_VIBuffer()
{
	//10   100   111
	//6    36    43    7
	//5    25    31    6
	//정점 개수
	m_iNumVertices = 31;
	//한 정점 벡터의 크기?
	m_iStride = sizeof(VTXCOL);

	const float PI = acos(-1);
	const int M = 5;
	const int N = 5;
	float rad = 1.f;

	float delta_phi = (float)(PI / M);
	float delta_theta = (float)(2 * PI / N);

	//정점관리 벡터
	vector<VTXCOL>	vecVertices;
	//> 사이즈 조정
	vecVertices.resize(m_iNumVertices);

	for (int i = 0; i <= M; i++)
	{
		for (int j = 0; j <= N; j++)
		{

			float phi = delta_phi * i - (float)(PI / 2);
			float theta = delta_theta * j;

			vecVertices[(N*i) + j] = VTXCOL(XMFLOAT3(
				(rad * cosf(phi)) * cosf(theta),
				(rad * cosf(phi)) * (float)sinf(theta)
				, (rad * sinf(phi))), 
				XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));		
		}
	}

	//인덱스
	m_iNumIndices = M * N * 6;
	vector<_uint>	vecIndices;
	vecIndices.resize(m_iNumIndices);

	int index = 0;

	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			vecIndices[index++] = N * i + j;
			vecIndices[index++] = N * (i + 1) + j;
			vecIndices[index++] = N * i + (j + 1);

			vecIndices[index++] = N * i + (j + 1);
			vecIndices[index++] = N * (i + 1) + j;
			vecIndices[index++] = N * (i + 1) + (j + 1);
		}
	}


	////////////////////////////////////////////////////////////////////////////////////
	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);

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

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
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
	
		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
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

CBuffer_SphereCol* CBuffer_SphereCol::Create()
{
	CBuffer_SphereCol* pInstance = new CBuffer_SphereCol();

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_SphereCol Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_SphereCol::Clone_Component(void* pArg)
{
	return new CBuffer_SphereCol(*this);
}

void CBuffer_SphereCol::Free()
{
	CVIBuffer::Free();
}
