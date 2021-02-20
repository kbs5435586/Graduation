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
	//정점 개수
	m_iNumVertices = 400;
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
				//new Vertex4(new Vector4(Point3D[i, j].x,
				//Point3D[i, j].y, Point3D[i, j].z, 1.0f), col);
			vecIndices[index++] = N * (i + 1) + j;
				//new Vertex4(new Vector4(Point3D[i + 1, j].x,
				//Point3D[i + 1, j].y, Point3D[i + 1, j].z, 1.0f), col);
			vecIndices[index++] = N * i + (j + 1);
				//new Vertex4(new Vector4(Point3D[i, j + 1].x,
				//Point3D[i, j + 1].y, Point3D[i, j + 1].z, 1.0f), col);

			vecIndices[index++] = N * i + (j + 1);
				//new Vertex4(new Vector4(Point3D[i, j + 1].x,
				//Point3D[i, j + 1].y, Point3D[i, j + 1].z, 1.0f), col);
			vecIndices[index++] = N * (i + 1) + j;
				//new Vertex4(new Vector4(Point3D[i + 1, j].x,
				//Point3D[i + 1, j].y, Point3D[i + 1, j].z, 1.0f), col);
			vecIndices[index++] = N * (i + 1) + (j + 1);
				//new Vertex4(new Vector4(Point3D[i + 1, j + 1].x,
				//Point3D[i + 1, j + 1].y, Point3D[i + 1, j + 1].z, 1.0f), col);

		}
	}


	

	//const float PI = acos(-1);
	//
	//float radius = 1.f;
	//int sectorCnt = 5;
	//int stackCnt = 5;
	//
	//float x, y, z, xy;                              // vertex position
	//float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	//
	//float sectorStep = 2 * PI / sectorCnt;
	//float stackStep = PI / stackCnt;
	//float sectorAngle, stackAngle;
	//
	//for (int i = 0; i <= stackCnt; ++i)
	//{
	//	stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	//	xy = radius * cosf(stackAngle);             // r * cos(u)
	//	z = radius * sinf(stackAngle);              // r * sin(u)
	//
	//	// add (sectorCount+1) vertices per stack
	//	// the first and last vertices have same position and normal, but different tex coords
	//	for (int j = 0; j <= sectorCnt; ++j)
	//	{
	//		sectorAngle = j * sectorStep;           // starting from 0 to 2pi
	//
	//		// vertex position
	//		x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	//		y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
	//
	//		vecVertices[sectorCnt * i + j] = VTXCOL(XMFLOAT3(x, y, z), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
	//	}
	//}
	//
	////인덱스 개수
	////인덱스
	//m_iNumIndices = 1000;
	//vector<_uint>	vecIndices;
	////>사이즈 조정
	//unsigned int k1, k2;
	//for (int i = 0; i < stackCnt; ++i)
	//{
	//	k1 = i * (sectorCnt + 1);     // beginning of current stack
	//	k2 = k1 + sectorCnt + 1;      // beginning of next stack
	//
	//	for (int j = 0; j < sectorCnt; ++j, ++k1, ++k2)
	//	{
	//		// 2 triangles per sector excluding 1st and last stacks
	//		if (i != 0)
	//		{
	//
	//			vecIndices.push_back(k1);
	//			vecIndices.push_back(k2);
	//			vecIndices.push_back(k1 + 1);
	//			// k1---k2---k1+1
	//		}
	//
	//		if (i != (stackCnt - 1))
	//		{
	//			vecIndices.push_back(k1 + 1);
	//			vecIndices.push_back(k2);
	//			vecIndices.push_back(k2 + 1);
	//			// k1+1---k2---k2+1
	//		}
	//
	//		//// vertical lines for all stacks
	//		//lineIndices.push_back(k1);
	//		//lineIndices.push_back(k2);
	//		//if (i != 0)  // horizontal lines except 1st stack
	//		//{
	//		//	lineIndices.push_back(k1);
	//		//	lineIndices.push_back(k1 + 1);
	//		//}
	//	}
	//}


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
