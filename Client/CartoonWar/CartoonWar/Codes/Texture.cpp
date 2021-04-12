#include "framework.h"
#include "Texture.h"


CTexture::CTexture()
	: CComponent()
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_vecSrvDescriptorIncrementSize(rhs.m_vecSrvDescriptorIncrementSize)
	, m_vecTextureUpload(rhs.m_vecTextureUpload)
	, m_vecTexture(rhs.m_vecTexture)
	, m_vecSRV(rhs.m_vecSRV)
	, m_pSRV(rhs.m_pSRV)
	, m_vecUAV(rhs.m_vecUAV)
	, m_mapSrvDescHeap(rhs.m_mapSrvDescHeap)
{
	m_IsClone = true;
	for (auto& iter : m_vecTexture)
		iter->AddRef();
	for (auto& iter : m_vecTextureUpload)
		iter->AddRef();
	for (auto& iter : m_vecSRV)
		iter->AddRef();
	for (auto& iter : m_vecUAV)
		iter->AddRef();
	for (auto& iter : m_mapSrvDescHeap)
	{
		for (auto& iter1 : iter.second)
		{
			iter1->AddRef();
		}
	}

}


HRESULT CTexture::Ready_Texture(const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube)
{

	_tchar	szFilePath[MAX_PATH] = L"";

	CDevice::GetInstance()->Open();

	if (eType == TEXTURE_TYPE::TEXTURE_TYPE_DDS)
	{
		for (_uint i = 0; i < iNum; ++i)
		{
			wsprintf(szFilePath, pFilepath, i);

			if (FAILED(LoadFromDDSFile(szFilePath, DDS_FLAGS_NONE, nullptr, m_Image)))
				return E_FAIL;
			if (FAILED(Create_ShaderResourceView(m_Image, IsCube)))
				return E_FAIL;

		}
	}
	else if (eType == TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG)
	{
		for (_uint i = 0; i < iNum; ++i)
		{
			wsprintf(szFilePath, pFilepath, i);
			if (FAILED(LoadFromWICFile(szFilePath, WIC_FLAGS_NONE, nullptr, m_Image)))
				return E_FAIL;
			if (FAILED(Create_ShaderResourceView(m_Image, IsCube)))
				return E_FAIL;
		}

	}
	else if (eType == TEXTURE_TYPE::TEXTURE_TGA)
	{

		for (_uint i = 0; i < iNum; ++i)
		{
			wsprintf(szFilePath, pFilepath, i);

			if (FAILED(LoadFromTGAFile(szFilePath, nullptr, m_Image)))
				return E_FAIL;
			if (FAILED(Create_ShaderResourceView(m_Image, IsCube)))
				return E_FAIL;
		}
	}



	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForFenceEvent();

	return S_OK;
}

HRESULT CTexture::Ready_Texture(const _tchar* pTag, const _tchar* pFilePath)
{
	wchar_t szExt[50] = L"";
	_wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

	wstring strExt = szExt;

	CDevice::GetInstance()->Open();

	if (L".dds" == strExt || L".DDS" == strExt)
	{
		if (FAILED(LoadFromDDSFile(pFilePath, DDS_FLAGS_NONE, nullptr, m_Image)))
			return E_FAIL;

	}
	else if (L".tga" == strExt || L".TGA" == strExt)
	{
		if (FAILED(LoadFromTGAFile(pFilePath, nullptr, m_Image)))
			return E_FAIL;
	}
	else
	{

		if (FAILED(LoadFromWICFile(pFilePath, WIC_FLAGS_NONE, nullptr, m_Image)))
			return E_FAIL;
	}

	if (FAILED(Create_ShaderResourceView(m_Image, m_vecSRV)))
		return E_FAIL;
	
	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForFenceEvent();

	auto iter = m_mapSrvDescHeap.find(pTag);
	if (iter == m_mapSrvDescHeap.end())
	{		
		m_mapSrvDescHeap.insert({pTag, m_vecSRV });
	}
	else
	{
		iter->second.push_back(m_vecSRV.back());
	}

	return S_OK;
}

HRESULT CTexture::Ready_Texture(const _tchar* pFilePath)
{
	wchar_t szExt[50] = L"";
	_wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

	wstring strExt = szExt;

	CDevice::GetInstance()->Open();

	if (L".dds" == strExt || L".DDS" == strExt)
	{
		if (FAILED(LoadFromDDSFile(pFilePath, DDS_FLAGS_NONE, nullptr, m_Image)))
			return E_FAIL;

	}
	else if (L".tga" == strExt || L".TGA" == strExt)
	{
		if (FAILED(LoadFromTGAFile(pFilePath, nullptr, m_Image)))
			return E_FAIL;
	}
	else
	{

		if (FAILED(LoadFromWICFile(pFilePath, WIC_FLAGS_NONE, nullptr, m_Image)))
			return E_FAIL;
	}

	if (FAILED(Create_ShaderResourceView_(m_Image, m_pSRV)))
		return E_FAIL;

	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForFenceEvent();

	return S_OK;
}


CTexture* CTexture::Create(const _tchar* pFilePath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube)
{
	CTexture* pInstance = new CTexture();

	if (FAILED(pInstance->Ready_Texture(pFilePath, iNum, eType, IsCube)))
	{
		MessageBox(0, L"CTexture Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}
CTexture* CTexture::Create(const _tchar* pTag, const _tchar* pFilePath)
{
	CTexture* pInstance = new CTexture();

	if (FAILED(pInstance->Ready_Texture(pTag, pFilePath)))
	{
		MessageBox(0, L"CTexture Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}
CTexture* CTexture::Create(const _tchar* pFilePath)
{
	CTexture* pInstance = new CTexture();

	if (FAILED(pInstance->Ready_Texture( pFilePath)))
	{
		MessageBox(0, L"CTexture Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}
HRESULT CTexture::Create_ShaderResourceView(ScratchImage& Image, _bool IsCube)
{
	ID3D12Resource* pTexture = nullptr;
	ID3D12Resource* pTextureUpload = nullptr;
	ID3D12DescriptorHeap* pDescHeap = nullptr;
	if (FAILED(CreateTexture(CDevice::GetInstance()->GetDevice().Get(), Image.GetMetadata(), &pTexture)))
		return E_FAIL;

	vector<D3D12_SUBRESOURCE_DATA> vecSubresources;

	if (FAILED(PrepareUpload(CDevice::GetInstance()->GetDevice().Get()
		, m_Image.GetImages(), m_Image.GetImageCount(), Image.GetMetadata(), vecSubresources)))
		return E_FAIL;

	const _uint UploadBufferSize = GetRequiredIntermediateSize(pTexture, 0, (_uint)vecSubresources.size());

	CD3DX12_HEAP_PROPERTIES tUploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC	tDesc = CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tUploadHeap,
		D3D12_HEAP_FLAG_NONE, &tDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pTextureUpload))))
		return E_FAIL;


	UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get()
		, pTexture, pTextureUpload, 0, 0
		, static_cast<unsigned int>(vecSubresources.size()), vecSubresources.data());


	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	ZeroMemory(&srvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvHeapDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pDescHeap));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = pDescHeap->GetCPUDescriptorHandleForHeapStart();

	_uint iSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	if (!IsCube)
	{
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_Image.GetMetadata().format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
	}
	else
	{
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_Image.GetMetadata().format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}

	CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, handle);

	m_vecSRV.push_back(pDescHeap);
	m_vecTexture.push_back(pTexture);
	m_vecTextureUpload.push_back(pTextureUpload);
	m_vecSrvDescriptorIncrementSize.push_back(iSize);

	return S_OK;
}
HRESULT CTexture::Create_ShaderResourceView(ScratchImage& Image, vector<ID3D12DescriptorHeap*>& vecDesc)
{
	ID3D12DescriptorHeap* pDesc = nullptr;
	ID3D12Resource* pTexture = nullptr;
	ID3D12Resource* pTextureUpload = nullptr;

	if (FAILED(CreateTexture(CDevice::GetInstance()->GetDevice().Get(), Image.GetMetadata(), &pTexture)))
		return E_FAIL;

	vector<D3D12_SUBRESOURCE_DATA> vecSubresources;

	if (FAILED(PrepareUpload(CDevice::GetInstance()->GetDevice().Get()
		, m_Image.GetImages(), m_Image.GetImageCount(), Image.GetMetadata(), vecSubresources)))
		return E_FAIL;

	const _uint UploadBufferSize = GetRequiredIntermediateSize(pTexture, 0, (_uint)vecSubresources.size());

	CD3DX12_HEAP_PROPERTIES tUploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC	tDesc = CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tUploadHeap,
		D3D12_HEAP_FLAG_NONE, &tDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pTextureUpload))))
		return E_FAIL;


	UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get()
		, pTexture, pTextureUpload, 0, 0
		, static_cast<unsigned int>(vecSubresources.size()), vecSubresources.data());


	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	ZeroMemory(&srvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvHeapDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pDesc));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = pDesc->GetCPUDescriptorHandleForHeapStart();

	_uint iSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Image.GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, handle);

	vecDesc.push_back(pDesc);
	m_vecTexture.push_back(pTexture);
	m_vecTextureUpload.push_back(pTextureUpload);
	m_vecSrvDescriptorIncrementSize.push_back(iSize);

	return S_OK;
}
HRESULT CTexture::Create_ShaderResourceView_(ScratchImage& Image, ComPtr<ID3D12DescriptorHeap>& pSrv)
{
	ID3D12Resource* pTexture = nullptr;
	ID3D12Resource* pTextureUpload = nullptr;

	if (FAILED(CreateTexture(CDevice::GetInstance()->GetDevice().Get(), Image.GetMetadata(), &pTexture)))
		return E_FAIL;

	vector<D3D12_SUBRESOURCE_DATA> vecSubresources;

	if (FAILED(PrepareUpload(CDevice::GetInstance()->GetDevice().Get()
		, m_Image.GetImages(), m_Image.GetImageCount(), Image.GetMetadata(), vecSubresources)))
		return E_FAIL;

	const _uint UploadBufferSize = GetRequiredIntermediateSize(pTexture, 0, (_uint)vecSubresources.size());

	CD3DX12_HEAP_PROPERTIES tUploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC	tDesc = CD3DX12_RESOURCE_DESC::Buffer(UploadBufferSize);

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tUploadHeap,
		D3D12_HEAP_FLAG_NONE, &tDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pTextureUpload))))
		return E_FAIL;


	UpdateSubresources(CDevice::GetInstance()->GetCmdLst().Get()
		, pTexture, pTextureUpload, 0, 0
		, static_cast<unsigned int>(vecSubresources.size()), vecSubresources.data());


	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	ZeroMemory(&srvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvHeapDesc.NodeMask = 0;
	CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pSrv));

	D3D12_CPU_DESCRIPTOR_HANDLE handle = pSrv->GetCPUDescriptorHandleForHeapStart();

	_uint iSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_Image.GetMetadata().format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, handle);

	m_vecTexture.push_back(pTexture);
	m_vecTextureUpload.push_back(pTextureUpload);
	m_vecSrvDescriptorIncrementSize.push_back(iSize);

	return S_OK;
}
HRESULT CTexture::SetUp_OnShader(_int iIdx, TEXTURE_REGISTER eRegister)	//0 t1
{
	if (m_vecSRV.size() <= iIdx)
		return E_FAIL;


	CD3DX12_GPU_DESCRIPTOR_HANDLE hTexture
	(
		m_vecSRV[iIdx]->GetGPUDescriptorHandleForHeapStart()
	);

 	hTexture.Offset(m_vecSrvDescriptorIncrementSize[iIdx]*(_uint)eRegister);

	CDevice::GetInstance()->GetCmdLst()->SetDescriptorHeaps(1, &m_vecSRV[iIdx]);
	CDevice::GetInstance()->GetCmdLst()->SetGraphicsRootDescriptorTable(0, hTexture);

	return S_OK;
}
CComponent* CTexture::Clone_Component(void* pArg)
{
	return new CTexture(*this);
}
void CTexture::Free()
{
	for (auto& iter : m_vecTexture)
		Safe_Release(iter);
	for (auto& iter : m_vecTextureUpload)
		Safe_Release(iter);
	for (auto& iter : m_vecSRV)
		Safe_Release(iter);
	for (auto& iter : m_vecUAV)
		Safe_Release(iter);

	for (auto& iter : m_mapSrvDescHeap)
		for (auto& iter1 : iter.second)
			Safe_Release(iter1);

	m_vecTexture.clear();
	m_vecTextureUpload.clear();
	m_vecSRV.clear();
	m_mapSrvDescHeap.clear();

	m_vecTexture.shrink_to_fit();
	m_vecTextureUpload.shrink_to_fit();
	m_vecSRV.shrink_to_fit();
	m_vecUAV.shrink_to_fit();
	

	CComponent::Free();
}
