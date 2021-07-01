#include "framework.h"
#include "Buffer_Sphere.h"
#include "Management.h"

CBuffer_Sphere::CBuffer_Sphere()
	: CVIBuffer()
{
}

CBuffer_Sphere::CBuffer_Sphere(const CBuffer_Sphere& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_Sphere::Ready_VIBuffer()
{
	m_iStride = sizeof(VTXCOL);

	const float PI = acos(-1);
	UINT iStackCount = 40; // 가로 분할 개수
	UINT iSliceCount = 40; // 세로 분할 개수
	float fRadius = 1.f;

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	m_iNumVertices = iStackCount * iSliceCount + iSliceCount + 1;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;


	//정점관리 벡터
	vector<VTXTEXNOR>	vecVertices;
	//> 사이즈 조정

	VTXTEXNOR v;

	v.vPos = _vec3(0.f, fRadius, 0.f);
	v.vTexUV = _vec2(0.5f, 0.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	vecVertices.push_back(v);


	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;
			
			v.vPos = _vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vTexUV = _vec2(fUVXStep * j, fUVYStep * i);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();
		
			vecVertices.push_back(v);
		}
	}

	v.vPos = _vec3(0.f, -fRadius, 0.f);
	v.vTexUV = _vec2(0.5f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	vecVertices.push_back(v);

	vector<_uint>	vecIndices;
	//인덱스
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIndices.push_back(0);
		vecIndices.push_back(i + 2);
		vecIndices.push_back(i + 1);
	}

	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIndices.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIndices.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIndices.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIndices.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIndices.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIndices.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	UINT iBottomIdx = (UINT)vecVertices.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIndices.push_back(iBottomIdx);
		vecIndices.push_back(iBottomIdx - (i + 2));
		vecIndices.push_back(iBottomIdx - (i + 1));
	}

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * vecVertices.size());

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
		vertexData.RowPitch = m_iStride * vecVertices.size();
		vertexData.SlicePitch = m_iStride * vecVertices.size();

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * vecIndices.size());


		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
			return E_FAIL;
		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
			return E_FAIL;

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = (void*)(vecIndices.data());
		indexData.RowPitch = sizeof(_uint) * vecIndices.size();
		indexData.SlicePitch = sizeof(_uint) * vecIndices.size();

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();


	m_tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_tVertexBufferView.StrideInBytes = m_iStride;
	m_tVertexBufferView.SizeInBytes = m_iStride * vecVertices.size();

	m_tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_tIndexBufferView.SizeInBytes = sizeof(_uint) * vecIndices.size();

	CDevice::GetInstance()->WaitForFenceEvent();
	return S_OK;
}



CBuffer_Sphere* CBuffer_Sphere::Create()
{
	CBuffer_Sphere* pInstance = new CBuffer_Sphere();
	if (FAILED(pInstance->Ready_VIBuffer()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CBuffer_Sphere::Clone_Component(void* pArg)
{
	return new CBuffer_Sphere(*this);
}

void CBuffer_Sphere::Free()
{
	CVIBuffer::Free();
}
