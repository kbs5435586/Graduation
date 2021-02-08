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

	D3D12_DESCRIPTOR_HEAP_DESC tDesc = {};
	tDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	tDesc.NumDescriptors = 1;
	tDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	tDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&tDesc, IID_PPV_ARGS(&m_pRTV));

	D3D12_RESOURCE_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;

	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = m_iTextureWidth;
	resourceDesc.Height = m_iTextureHeight;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Format = CDevice::GetInstance()->GetSwapChainFormat(CDevice::GetInstance()->GetBitDepth());

	D3D12_CLEAR_VALUE tClearVal;
	tClearVal.Color[0] = m_fClearColor[0];
	tClearVal.Color[1] = m_fClearColor[1];
	tClearVal.Color[2] = m_fClearColor[2];
	tClearVal.Color[3] = m_fClearColor[3];
	tClearVal.Format = CDevice::GetInstance()->GetSwapChainFormat(CDevice::GetInstance()->GetBitDepth());

	CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&heapProperty,
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET,
		&tClearVal, IID_PPV_ARGS(m_pTexture.GetAddressOf()) )))
		return E_FAIL;

	//Create Render Target View
	D3D12_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Texture2D.MipSlice = 0;
	desc.Texture2D.PlaneSlice = 0;

	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Format = CDevice::GetInstance()->GetSwapChainFormat(CDevice::GetInstance()->GetBitDepth());

	D3D12_CPU_DESCRIPTOR_HANDLE hRTVHeap = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	CDevice::GetInstance()->GetDevice()->CreateRenderTargetView(m_pTexture.Get(), &desc, hRTVHeap);

	//Device Copy Desc 

	//Create Shader Resource View
	D3D12_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Texture2D.MipLevels = resourceDesc.MipLevels;
	descSRV.Texture2D.MostDetailedMip = 0;
	descSRV.Format = resourceDesc.Format;
	descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	descSRV.Format = CDevice::GetInstance()->GetSwapChainFormat(CDevice::GetInstance()->GetBitDepth());

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	ZeroMemory(&srvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvHeapDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRV));

	D3D12_CPU_DESCRIPTOR_HANDLE hSRVHeap = m_pSRV->GetCPUDescriptorHandleForHeapStart();
	CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_pTexture.Get(), &descSRV, hSRVHeap);

	if (FAILED(Ready_Component()))
		return E_FAIL;
	if (FAILED(Crate_InputLayOut()))
		return E_FAIL;

	

	//D3D12_CPU_DESCRIPTOR_HANDLE hDescHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	//hDescHandle.ptr/* += RTV HeapSize */;

	//D3D12_CPU_DESCRIPTOR_HANDLE hSrcHandle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	//hSrcHandle.ptr;

	//UINT iDestRange = (UINT)TEXTURE_REGISTER::END;
	//UINT iSrcRange = (UINT)TEXTURE_REGISTER::END;

	//CDevice::GetInstance()->GetDevice()->CopyDescriptors(1, &hDescHandle, &iDestRange, 
	//	1, &hSrcHandle, &iSrcRange, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// Omset RT °¹¼ö, handle, 

	return S_OK;
}

void CRTT::Set_RenderTarget(ID3D12DescriptorHeap* pDsv)
{
	float arr[4] = { 0.f,0.f,1.f,1.f };
	CDevice::GetInstance()->Open();

	D3D12_VIEWPORT tViewPor = D3D12_VIEWPORT{ 0.f, 0.f, 50, 50, 0.f, 1.f };
	D3D12_RECT m_tScissorRect = D3D12_RECT{ 0, 0, (LONG)50, (LONG)50 };
	CDevice::GetInstance()->GetCmdLst()->RSSetViewports(1, &tViewPor);
	CDevice::GetInstance()->GetCmdLst()->RSSetScissorRects(1, &m_tScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRTV->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE dHandle = pDsv->GetCPUDescriptorHandleForHeapStart();

	CDevice::GetInstance()->GetCmdLst()->OMSetRenderTargets(1, &handle, FALSE, &dHandle);
	CDevice::GetInstance()->GetCmdLst()->ClearRenderTargetView(handle, arr, 0, nullptr);
	CDevice::GetInstance()->GetCmdLst()->ClearDepthStencilView(pDsv->GetCPUDescriptorHandleForHeapStart(), 
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);




	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootSignature(
		CDevice::GetInstance()->GetRootSignature(ROOT_SIG_TYPE::RENDER).Get());
	CDevice::GetInstance()->GetCmdLst()->SetPipelineState(m_pShaderCom->GetPipeLine().Get());

	CDevice::GetInstance()->SetTextureToShader(m_pSRV.Get(),  TEXTURE_REGISTER::t0);
	m_pBufferCom->Render_VIBuffer();

	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForFenceEvent();
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
		Safe_Release(pInstance);
	return pInstance;
}

void CRTT::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
}
