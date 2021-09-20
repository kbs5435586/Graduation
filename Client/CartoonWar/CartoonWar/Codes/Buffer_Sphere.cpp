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
	_uint iStackCount = 40; // 가로 분할 개수
	_uint iSliceCount = 40; // 세로 분할 개수
	float fRadius = 2.f;

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	m_iNumVertices = iStackCount * iSliceCount + iSliceCount + 1;

	vector< VTXCOL > verts;
	_uint count = 0;
	while (count < iSliceCount)
	{
		const float phi = XM_PI / iStackCount;
		_uint count2 = 0;
		while (count2 < iStackCount)
		{
			const float theta = XM_2PI / iSliceCount;
			const float xzRadius = fabsf(fRadius * cosf(phi));

			VTXCOL v;
			v.vPosition.x= xzRadius * cosf(theta);
			v.vPosition.y = fRadius * sinf(phi);
			v.vPosition.z = xzRadius * sinf(theta);

			const float fRcpLen = 1.0f / sqrtf((v.vPosition.x * v.vPosition.x) + (v.vPosition.y * v.vPosition.y) + (v.vPosition.z * v.vPosition.z));
			v.vNormal.x = v.vPosition.x * fRcpLen;
			v.vNormal.y = v.vPosition.y * fRcpLen;
			v.vNormal.z = v.vPosition.z * fRcpLen;

			v.vColor = _vec4(1.f,1.f,1.f,1.f);

			verts.push_back(v);
			count2++;
		}
		count++;
	}
	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CDevice::GetInstance()->Open();
	{
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(VTXCOL) * 3);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");


		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(verts.data());
		vertexData.RowPitch = sizeof(VTXCOL) * 3;
		vertexData.SlicePitch = sizeof(VTXCOL) * 3;

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	CDevice::GetInstance()->Close();


	m_tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_tVertexBufferView.StrideInBytes = sizeof(VTXCOL);
	m_tVertexBufferView.SizeInBytes = sizeof(VTXCOL) * 3;
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
