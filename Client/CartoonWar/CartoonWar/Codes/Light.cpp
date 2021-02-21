#include "framework.h"
#include "Light.h"
#include "Shader.h"
#include "Management.h"
CLight::CLight()
{

}

HRESULT CLight::Ready_Light(const LIGHT& tLightInfo)
{
	m_tLight = tLightInfo;

	/*D3D12_VIEWPORT tViewPort = CDevice::GetInstance()->GetViewPort();

	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vector<VTXTEX>		vecVertices;
	vecVertices.resize(m_iNumVertices);
	vecVertices[0] = VTXTEX(XMFLOAT3(0.f - 0.5f, 0.0f - 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f));
	vecVertices[1] = VTXTEX(XMFLOAT3(tViewPort.Width - 0.5f, 0.0f - 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f));
	vecVertices[2] = VTXTEX(XMFLOAT3(tViewPort.Width - 0.5f, tViewPort.Height - 0.5f, 0.0f), XMFLOAT2(1.f, 1.f));
	vecVertices[3] = VTXTEX(XMFLOAT3(0.0f - 0.5f, tViewPort.Height - 0.5f, 0.0f), XMFLOAT2(0.0f, 1.f));


	m_iNumIndices = 6;

	vector<_uint>	vecIndices;
	vecIndices.resize(m_iNumIndices);
	vecIndices[0] = 0; vecIndices[1] = 1; vecIndices[2] = 2;
	vecIndices[3] = 0; vecIndices[4] = 2; vecIndices[5] = 3;


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
		vertexData.RowPitch = m_iStride * m_iNumIndices;;
		vertexData.SlicePitch = m_iStride * m_iNumIndices;

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

	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Deffered_Light.hlsl", "VS_Main", "PS_Main");
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(Create_InputLayOut()))
		return E_FAIL;*/

	return S_OK;
}

void CLight::Render_Light()
{
	/*CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return;

	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pShaderCom->GetPipeLine().Get());

	ComPtr<ID3D12DescriptorHeap>	pTextureDesc0 = pManagement->Get_RTT((_uint)MRT::MRT_DEFFERD)->Get_RTT(1)->pRtt->GetSRV().Get();
	CDevice::GetInstance()->SetTextureToShader(pTextureDesc0.Get(), TEXTURE_REGISTER::t0);
	CDevice::GetInstance()->UpdateTable();

	CDevice::GetInstance()->GetCmdLst()->IASetPrimitiveTopology(m_PrimitiveTopology);
	CDevice::GetInstance()->GetCmdLst()->IASetVertexBuffers(m_iSlot, 1, &m_tVertexBufferView);
	CDevice::GetInstance()->GetCmdLst()->IASetIndexBuffer(&m_tIndexBufferView);
	CDevice::GetInstance()->GetCmdLst()->DrawIndexedInstanced(m_iNumIndices, 1, 0, 0, 0);*/
}

HRESULT CLight::Create_InputLayOut()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS, SHADER_TYPE::SHADER_LIGHT)))
		return E_FAIL;
	return S_OK;
}

CLight* CLight::Create(const LIGHT& tLightInfo)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Ready_Light(tLightInfo)))
	{
		MessageBox(0, L"CLight Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pShaderCom);
	//Safe_Delete(m_pLight);
}
