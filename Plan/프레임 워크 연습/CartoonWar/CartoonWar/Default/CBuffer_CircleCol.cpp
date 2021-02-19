#include "framework.h"
#include "CBuffer_CircleCol.h"

CBuffer_CircleCol::CBuffer_CircleCol()
	: CVIBuffer()
{
}

CBuffer_CircleCol::CBuffer_CircleCol(const CBuffer_CircleCol& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CBuffer_CircleCol::Ready_VIBuffer()
{
	//���� ����
	//m_iNumVertices = 4;
	m_iNumVertices = 60;
	float rad = 10.f;
	//�� ���� ������ ũ��?
	m_iStride = sizeof(VTXCOL);

	//VTXCOL�� ��� ����
	vector<VTXCOL>	vecVertices;
	vecVertices.resize(m_iNumVertices);

	//���� �ʱ�ȭ
	//vecVertices[0] = VTXCOL(XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
	//vecVertices[1] = VTXCOL(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
	//vecVertices[2] = VTXCOL(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));

	for (int i = 0; i < 10; ++i)
	{

		vecVertices[3 * i] = VTXCOL(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
		vecVertices[3 * i + 1] = VTXCOL(XMFLOAT3(rad * cos(36 * i * 3.141592 / 180),
			rad * sin(36 * i * 3.141592 / 180), 0.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
		vecVertices[3 * i + 2] = VTXCOL(XMFLOAT3(rad * cos(36 * (i + 1) * 3.141592 / 180),
			rad * sin(36 * (i + 1) * 3.141592 / 180), 0.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
	}

	for (int i = 10; i < 20; ++i)
	{

		vecVertices[3 * i] = VTXCOL(XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
		vecVertices[3 * i + 1] = VTXCOL(XMFLOAT3(rad * cos(36 * i * 3.141592 / 180),
			rad * sin(36 * i * 3.141592 / 180), 1.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));
		vecVertices[3 * i + 2] = VTXCOL(XMFLOAT3(rad * cos(36 * (i + 1) * 3.141592 / 180),
			rad * sin(36 * (i + 1) * 3.141592 / 180), 1.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.f));

	}

	//�ε���
	//�ð����
	//m_iNumIndices = 6;
	//vector<_uint>	vecIndices;
	//vecIndices.resize(m_iNumIndices);
	//
	//vecIndices[0] = 0;
	//vecIndices[1] = 2;
	//vecIndices[2] = 1;
	//vecIndices[3] = 0;
	//vecIndices[4] = 3;
	//vecIndices[5] = 2;

	//vecIndices[0] = 0;
	//vecIndices[1] = 1;
	//vecIndices[2] = 3;
	//vecIndices[3] = 2;
	//vecIndices[4] = 3;
	//vecIndices[5] = 1;

	D3D12_HEAP_PROPERTIES tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_HEAP_PROPERTIES tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	CDevice::GetInstance()->Open();
	{
		//�������� ũ�� * �ε��� ���� ������ŭ �� 
		D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(m_iStride * m_iNumVertices);

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pVertexBuffer))))
			return E_FAIL;
		m_pVertexBuffer->SetName(L"VertexBuffer");

		if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
			&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pVertexUploadBuffer))))
			return E_FAIL;
		m_pVertexUploadBuffer->SetName(L"Upload VertexBuffer");
		//���� �Ⱥ��� �ɰŰ���


		//���� �������� ���ο� ���𰡸� ������
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = (void*)(vecVertices.data());
		//vector�� ���� .data() �Լ��� ���� ���ο��� ���Ҹ� �����ϴ� �迭�� ���� �����͸� ��ȯ�Ѵ�.
		vertexData.RowPitch = m_iStride * m_iNumVertices;
		vertexData.SlicePitch = m_iStride * m_iNumVertices;
		//���� �ΰ��� ���� ��� LONG_PTR�̶�� ������ ����Ǿ��ִ�. PTR�̶�� �̸��� ���� ��ŭ �ּҸ� �����ϴ� ���� �ϰ��̴�?
		//�ٵ� �� ���� ũ��� �ε��� ������ ���߳� �װ͵� �� ��?

		D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pVertexBuffer.Get(), m_pVertexUploadBuffer.Get(), 0, 0, 1, &vertexData);
		CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	}
	//{
	//	D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(_uint) * m_iNumIndices);
	//
	//
	//	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default, D3D12_HEAP_FLAG_NONE,
	//		&tResource_Desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pIndexBuffer))))
	//		return E_FAIL;
	//	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE, 
	//		&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pIndexUploadBuffer))))
	//		return E_FAIL;
	//
	//	D3D12_SUBRESOURCE_DATA indexData = {};
	//	indexData.pData = (void*)(vecIndices.data());
	//	indexData.RowPitch = sizeof(_uint) * m_iNumIndices;
	//	indexData.SlicePitch = sizeof(_uint) * m_iNumIndices;
	//
	//	D3D12_RESOURCE_BARRIER	tResource_Barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
	//	UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get(), m_pIndexBuffer.Get(), m_pIndexUploadBuffer.Get(), 0, 0, 1, &indexData);
	//	CDevice::GetInstance()->GetCmdLst()->ResourceBarrier(1, &tResource_Barrier);
	//}
	CDevice::GetInstance()->Close();


	m_tVertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_tVertexBufferView.StrideInBytes = m_iStride;
	m_tVertexBufferView.SizeInBytes = m_iStride * m_iNumVertices;

	//m_tIndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	//m_tIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//m_tIndexBufferView.SizeInBytes = sizeof(_uint) * m_iNumIndices;

	CDevice::GetInstance()->WaitForFenceEvent();
	return S_OK;
}

CBuffer_CircleCol* CBuffer_CircleCol::Create()
{
	CBuffer_CircleCol* pInstance = new CBuffer_CircleCol();

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		MessageBox(0, L"CBuffer_CircleCol Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBuffer_CircleCol::Clone_Component(void* pArg)
{
	return new CBuffer_CircleCol(*this);
}

void CBuffer_CircleCol::Free()
{
	CVIBuffer::Free();
}

