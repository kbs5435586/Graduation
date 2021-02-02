#include "framework.h"
#include "RTT.h"
#include "Management.h"


CRTT::CRTT()
{
}

HRESULT CRTT::Ready_RTT(_uint iTextureWidth, _uint iTextureHeight)
{
	m_iTextureWidth = iTextureWidth;
	m_iTextureHeight= iTextureHeight;
	ComPtr<ID3D12Resource> textureUploadHeap;

	D3D12_RESOURCE_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(D3D12_RESOURCE_DESC));

	tDesc.MipLevels = 1;
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tDesc.Width = m_iTextureWidth;
	tDesc.Height = m_iTextureHeight;
	tDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	tDesc.DepthOrArraySize = 1;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	
	
	CD3DX12_HEAP_PROPERTIES tHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	if(FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
		&tHeap, D3D12_HEAP_FLAG_NONE, &tDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_pTexture))))
		return E_FAIL;

	const _uint UploadBufferSize = GetRequiredIntermediateSize(m_pTexture.Get(), 0, 1);

	CD3DX12_HEAP_PROPERTIES tUploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC	tDesc_ = CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tUploadHeap,
		D3D12_HEAP_FLAG_NONE, &tDesc_, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadHeap))))
		return E_FAIL;



	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	ZeroMemory(&srvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvHeapDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pRTV));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRTV->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, handle);


	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(Crate_InputLayOut()))
		return E_FAIL;

	return S_OK;
}

void CRTT::Set_RenderTarget(ID3D12DescriptorHeap* pDsv)
{
	float arr[4] = {0.f,0.f,1.f,1.f};
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dHandle = pDsv->GetCPUDescriptorHandleForHeapStart();
	CDevice::GetInstance()->GetCmdLst()->OMSetRenderTargets(1, &handle, FALSE, &dHandle);
	CDevice::GetInstance()->GetCmdLst()->ClearRenderTargetView(handle, arr, 0, nullptr);
	CDevice::GetInstance()->GetCmdLst()->ClearDepthStencilView(pDsv->GetCPUDescriptorHandleForHeapStart(), 
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);


	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pShaderCom->GetPipeLine().Get());


	CDevice::GetInstance()->SetTextureToShader(m_pRTV.Get(), 0, TEXTURE_REGISTER::t0);

	m_pBufferCom->Render_VIBuffer();
}

HRESULT CRTT::Ready_Component()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (nullptr == pManagement)
		return E_FAIL;
	pManagement->AddRef();

	m_pBufferCom = (CBuffer_RectTex*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Buffer_RectTex_RTT");
	NULL_CHECK_VAL(m_pBufferCom, E_FAIL);
	m_pShaderCom = (CShader*)pManagement->Clone_Component((_uint)SCENEID::SCENE_STATIC, L"Component_Shader_RTT");
	NULL_CHECK_VAL(m_pShaderCom, E_FAIL);

	Safe_Release(pManagement);
	return S_OK;
}

HRESULT CRTT::Crate_InputLayOut()
{
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });

	if (FAILED(m_pShaderCom->Create_Shader(vecDesc)))
		return E_FAIL;

	return S_OK;
}

CRTT* CRTT::Create(_uint iTextureWidth, _uint iTextureHeight)
{
	CRTT* pInstance = new CRTT();
	if (FAILED(pInstance->Ready_RTT(iTextureWidth, iTextureHeight)))
		return nullptr;
	return pInstance;
}

void CRTT::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
}
