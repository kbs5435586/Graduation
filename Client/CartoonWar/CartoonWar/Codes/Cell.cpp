#include "framework.h"
#include "Cell.h"
#include "Line.h"
#include "Shader.h"
#include "Management.h"


CCell::CCell()
{
	ZeroMemory(m_pNeighbor, sizeof(CCell*) * (_uint)NEIGHBOR::NEIGHBOR_END);
	ZeroMemory(m_pLine, sizeof(CLine*) * (_uint)LINE::LINE_END);
	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Cell.hlsl", "VS_Main", "PS_Main");
	if (FAILED(Create_InputLayOut()))
		return ;
}

HRESULT CCell::Ready_Cell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIdx)
{
	m_vPoint[(_uint)POINT_::POINT_A] = *pPointA;
	m_vPoint[(_uint)POINT_::POINT_B] = *pPointB;
	m_vPoint[(_uint)POINT_::POINT_C] = *pPointC;

	m_iIndex = iIdx;

	m_pLine[(_uint)LINE::LINE_AB] = CLine::Create(m_vPoint[(_uint)POINT_::POINT_A], m_vPoint[(_uint)POINT_::POINT_B]);
	m_pLine[(_uint)LINE::LINE_BC] = CLine::Create(m_vPoint[(_uint)POINT_::POINT_B], m_vPoint[(_uint)POINT_::POINT_C]);
	m_pLine[(_uint)LINE::LINE_CA] = CLine::Create(m_vPoint[(_uint)POINT_::POINT_C], m_vPoint[(_uint)POINT_::POINT_A]);

	vector<VTXCOL>		vecVertices;
	vecVertices.resize(3);

	vecVertices[0].vPosition = m_vPoint[(_uint)POINT_::POINT_A];
	vecVertices[0].vColor = _vec4(1.f, 0.f, 0.f, 1.f);

	vecVertices[1].vPosition = m_vPoint[(_uint)POINT_::POINT_B];
	vecVertices[1].vColor = _vec4(1.f, 0.f, 0.f, 1.f);

	vecVertices[2].vPosition = m_vPoint[(_uint)POINT_::POINT_C];
	vecVertices[2].vColor = _vec4(1.f, 0.f, 0.f, 1.f);

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
		vertexData.pData = (void*)(vecVertices.data());
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

_bool CCell::Is_inCell(_vec3 vMovedPos, LINE* pOutLine)
{
	for (_uint i = 0; i < (_uint)LINE::LINE_END; ++i)
	{
		if (!m_pLine[i]->IsRight(vMovedPos))
		{
			*pOutLine = LINE(i);
			return false;
		}
	}
	return _bool(true);
}

_bool CCell::Compare_Point(_vec3* pPoint1, _vec3* pPoint2)
{

	if (m_vPoint[(_uint)POINT_::POINT_A] == *pPoint1)
	{
		if (m_vPoint[(_uint)POINT_::POINT_B] == *pPoint2)
			return true;

		if (m_vPoint[(_uint)POINT_::POINT_C] == *pPoint2)
			return true;
	}

	if (m_vPoint[(_uint)POINT_::POINT_B] == *pPoint1)
	{
		if (m_vPoint[(_uint)POINT_::POINT_A] == *pPoint2)
			return true;

		if (m_vPoint[(_uint)POINT_::POINT_C] == *pPoint2)
			return true;
	}

	if (m_vPoint[(_uint)POINT_::POINT_C] == *pPoint1)
	{
		if (m_vPoint[(_uint)POINT_::POINT_A] == *pPoint2)
			return true;

		if (m_vPoint[(_uint)POINT_::POINT_B] == *pPoint2)
			return true;
	}

	return _bool(false);
}

void CCell::Clear_Neighbor()
{
	for (auto& pNeighbor : m_pNeighbor)
	{

		Safe_Release(pNeighbor);
	}
}

void CCell::Render_Cell()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;
	pManagement->AddRef();


	MAINPASS tMainPass = {};
	_matrix matWorld = {};
	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();

	m_pShaderCom->SetUp_OnShader(matWorld, matView, matProj, tMainPass);

	_uint iOffeset = pManagement->GetConstantBuffer(0)->SetData((void*)&tMainPass);
	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer(0)->GetCBV().Get(), iOffeset, CONST_REGISTER::b0);
	CDevice::GetInstance()->UpdateTable();


	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(0, 1, &(m_tVertexBufferView));
	CDevice::GetInstance()->GetCmdLst()->DrawInstanced(3, 1, 0, 0);



	Safe_Release(pManagement);
}

void CCell::Set_Neighbor(NEIGHBOR eNeighbor, CCell* pCell)
{
	pCell->AddRef();
	m_pNeighbor[(_uint)eNeighbor] = pCell;
}

HRESULT CCell::Create_InputLayOut()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::WIREFRAME)))
		return E_FAIL;


	return S_OK;
}

CCell* CCell::Create(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC, const _uint& iIdx)
{
	CCell* pInstance = new CCell();
	if (FAILED(pInstance->Ready_Cell(pPointA, pPointB, pPointC, iIdx)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCell::Free()
{
	for (auto& pLine : m_pLine)
		Safe_Release(pLine);
	Safe_Release(m_pShaderCom);
}
