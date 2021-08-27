#include "framework.h"
#include "Buffer_Cone.h"
#define _USE_MATH_DEFINES
#include <math.h>


CBuffer_Cone::CBuffer_Cone()
	: CVIBuffer()
{
}

CBuffer_Cone::CBuffer_Cone(const CBuffer_Cone& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_Cone::Ready_VIBuffer()
{
	topRadius = 1.f;
	botRadius = 1.f;

	height = 1.f;

	sliceCount = 30;
	stackCount = 1;

	vector<VTXTEXCUBE> vecVertices;

	float stackHeight = height / (float)stackCount;

	float radiusStep = (topRadius - botRadius) / (float)stackCount;

	_uint ringCount = stackCount + 1;

	for (_uint i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = botRadius + i * radiusStep;

		float phi = 2.f *  M_PI / (float)sliceCount;


		for (_uint k = 0; k <= sliceCount; ++k)
		{
			float x = r * cosf(k * phi);
			float z = r * sinf(k * phi);

			VTXTEXCUBE vertex;
			vertex.vPosition = _vec3(x, y, z);
			vertex.vTex = vertex.vPosition;
			//_vec3 tangent = _vec3(-z, 0.f, x);
			//
			//float dr = botRadius - topRadius;
			//_vec3 biTangent = _vec3(dr * x, -height, dr * z);
			//
			//vertex.vTex = Vector3_::CrossProduct(tangent, biTangent);
			//vertex.vTex = Vector3_::Normalize(vertex.vTex);

			vecVertices.push_back(vertex);
		}
	}

	vector<_uint> indices;
	_uint ringVertexCount = sliceCount + 1;

	for (_uint y = 0; y < stackCount; ++y)
	{
		for (_uint x = 0; x < sliceCount; ++x)
		{
			indices.push_back(y * ringVertexCount + x);
			indices.push_back((y+1) * ringVertexCount + x);
			indices.push_back((y+1) * ringVertexCount + (x+1));

			indices.push_back(y * ringVertexCount + x);
			indices.push_back((y+1) * ringVertexCount + x + 1);
			indices.push_back(y * ringVertexCount + x + 1);
		}
	}

	//BuildTopCap(vecVertices, indices);
	//BuildBotCap(vecVertices, indices);

	//{
	//	float y = 0.5f * height;

	//	float phi = 2.f * M_PI / (float)sliceCount;

	//	for (_uint i = 0; i <= sliceCount; ++i)
	//	{
	//		float x = topRadius * cosf(i * phi);
	//		float z = topRadius * sinf(i * phi);

	//		float u = x / height + 0.5f;
	//		float v = z / height + 0.5f;

	//		vecVertices.push_back(VTXTEXCUBE(XMFLOAT3(x, y, z), XMFLOAT3(x, y, z)));
	//		//vertices.push_back(VTXTEXCUBE(x, y, z, 0, -1, 0));
	//	}
	//	vecVertices.push_back(VTXTEXCUBE(XMFLOAT3(0, y, 0), XMFLOAT3(0, y, 0)));
	//	//vertices.push_back(VTXTEXCUBE(0, y, 0, 0, -1, 0));


	//	_uint baseIndex = vecVertices.size() - sliceCount - 2;

	//	_uint centerIndex = vecVertices.size() - 1;

	//	for (_uint i = 0; i < sliceCount; ++i)
	//	{
	//		indices.push_back(centerIndex);
	//		indices.push_back(baseIndex + i + 1);
	//		indices.push_back(baseIndex + i);
	//	}
	//}

	//{
	//	float y = 0.5f * height;

	//	float phi = 2.f * M_PI / (float)sliceCount;

	//	for (_uint i = 0; i <= sliceCount; ++i)
	//	{
	//		float x = topRadius * cosf(i * phi);
	//		float z = topRadius * sinf(i * phi);

	//		float u = x / height + 0.5f;
	//		float v = z / height + 0.5f;

	//		vecVertices.push_back(VTXTEXCUBE(XMFLOAT3(x, y, z), XMFLOAT3(x, y, z)));
	//		//vertices.push_back(VTXTEXCUBE(x, y, z, 0, 1, 0));
	//	}
	//	vecVertices.push_back(VTXTEXCUBE(XMFLOAT3(0, y, 0), XMFLOAT3(0, y, 0)));
	//	//vertices.push_back(VTXTEXCUBE(0, y, 0, 0, 1, 0));


	//	_uint baseIndex = vecVertices.size() - sliceCount - 2;

	//	_uint centerIndex = vecVertices.size() - 1;

	//	for (_uint i = 0; i < sliceCount; ++i)
	//	{
	//		indices.push_back(centerIndex);
	//		indices.push_back(baseIndex + i + 1);
	//		indices.push_back(baseIndex + i);
	//	}
	//}
	
	

	_uint aaa = vecVertices.size();

	//정점 개수
	m_iNumVertices = aaa;

	float rad = 1.f;
	//한 정점 벡터의 크기?
	m_iStride = sizeof(VTXTEXCUBE);


	_uint bbb = indices.size();
	//인덱스
	//시계방향
	m_iNumIndices = bbb;
	



	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		//정점벡터 크기 * 인덱스 버퍼 개수만큼 곱 
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");



		//정점 데이터의 새로운 무언가를 생성함
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecVertices.data());
		//vector에 대한 .data() 함수는 벡터 내부에서 원소를 저장하는 배열에 대한 포인터를 반환한다.
		vertexData.RowPitch = m_iStride * m_iNumIndices;
		vertexData.SlicePitch = m_iStride * m_iNumIndices;
		//위의 두가지 변수 모두 LONG_PTR이라는 변수로 선언되어있다. PTR이라는 이름이 붙은 만큼 주소를 저장하는 변수 일것이다?
		//근데 왜 정점 크기랑 인덱스 개수를 곱했나 그것도 둘 다?

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
		indexData.pData = (void*)(indices.data());
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

CBuffer_Cone* CBuffer_Cone::Create()
{
	CBuffer_Cone* pInstance = new CBuffer_Cone();

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_Cone Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_Cone::Clone_Component(void* pArg)
{
	return new CBuffer_Cone(*this);
}

void CBuffer_Cone::Free()
{
	CVIBuffer::Free();
}

void CBuffer_Cone::BuildTopCap(vector<VTXTEXCUBE>& vertices, vector<_uint>& indices)
{
	float y = 0.5f * height;

	float phi = 2.f * M_PI / (float)sliceCount;

	for (_uint i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * phi);
		float z = topRadius * sinf(i * phi);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;
		
		vertices.push_back(VTXTEXCUBE(XMFLOAT3(x, y, z), XMFLOAT3(x, y, z)));
		//vertices.push_back(VTXTEXCUBE(x, y, z, 0, 1, 0));
	}
	vertices.push_back(VTXTEXCUBE(XMFLOAT3(0, y, 0), XMFLOAT3(0, y, 0)));
	//vertices.push_back(VTXTEXCUBE(0, y, 0, 0, 1, 0));


	_uint baseIndex = vertices.size() - sliceCount - 2;

	_uint centerIndex = vertices.size() - 1;

	for (_uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}

void CBuffer_Cone::BuildBotCap(vector<VTXTEXCUBE>& vertices, vector<_uint>& indices)
{
	float y = 0.5f * height;

	float phi = 2.f * M_PI / (float)sliceCount;

	for (_uint i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * phi);
		float z = topRadius * sinf(i * phi);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		vertices.push_back(VTXTEXCUBE(XMFLOAT3(x, y, z), XMFLOAT3(x, y, z)));
		//vertices.push_back(VTXTEXCUBE(x, y, z, 0, -1, 0));
	}
	vertices.push_back(VTXTEXCUBE(XMFLOAT3(0, y, 0), XMFLOAT3(0, y, 0)));
	//vertices.push_back(VTXTEXCUBE(0, y, 0, 0, -1, 0));


	_uint baseIndex = vertices.size() - sliceCount - 2;

	_uint centerIndex = vertices.size() - 1;

	for (_uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}

