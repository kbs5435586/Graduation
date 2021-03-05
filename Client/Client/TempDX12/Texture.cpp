#include "framework.h"
#include "Texture.h"
#include "DDSTextureLoader.h"

CTexture::CTexture(ID3D12Device* pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_vecTextureUpload(rhs.m_vecTextureUpload)
	, m_vecTexture(rhs.m_vecTexture)
	, m_vecDescriptorHeap(rhs.m_vecDescriptorHeap)
	, wicConverter(rhs.wicConverter)
	, wicFactory(rhs.wicFactory)
	, wicDecoder(rhs.wicDecoder)
	, wicFrame(rhs.wicFrame)
	, m_vecSrvDescriptorIncrementSize(rhs.m_vecSrvDescriptorIncrementSize)
{
	m_IsClone = true;
	for (auto& iter : m_vecTexture)
		iter->AddRef();
	for (auto& iter : m_vecTextureUpload)
		iter->AddRef();
	for (auto& iter : m_vecDescriptorHeap)
		iter->AddRef();
	{
		if (wicConverter)
			wicConverter->AddRef();
		if (wicFactory)
			wicFactory->AddRef();
		if (wicDecoder)
			wicDecoder->AddRef();
		if (wicFrame)
			wicFrame->AddRef();
	}

}


HRESULT CTexture::Ready_Texture(const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType)
{
	m_vecTexture.reserve(iNum);
	m_vecTextureUpload.reserve(iNum);
	m_vecDescriptorHeap.reserve(iNum);
	m_vecSrvDescriptorIncrementSize.reserve(iNum);

	ID3D12Resource* pTexture = nullptr;
	ID3D12Resource* pTextureUpload = nullptr;
	_tchar	szFilePath[MAX_PATH] = L"";

	CDevice::GetInstance()->Open();
	if (eType == TEXTURE_TYPE_DDS)
	{
		if (iNum == 0)
		{

			wsprintf(szFilePath, pFilepath, 0);

			if (FAILED(CreateDDSTextureFromFile12(CDevice::GetInstance()->GetDevice(),
				CDevice::GetInstance()->GetCommandList(), szFilePath, pTexture, pTextureUpload)))
			{
				return E_FAIL;
			}
			if (pTexture != nullptr)
			{
				m_vecTexture.push_back(pTexture);
				m_vecTextureUpload.push_back(pTextureUpload);
			}
			if (FAILED(Create_Shader_Resource_Heap()))
				return E_FAIL;
		}
		else
		{
			for (_uint i = 0; i < iNum; ++i)
			{
				wsprintf(szFilePath, pFilepath, i);

				if (FAILED(CreateDDSTextureFromFile12(CDevice::GetInstance()->GetDevice(),
					CDevice::GetInstance()->GetCommandList(), szFilePath, pTexture, pTextureUpload)))
					return E_FAIL;

				if (pTexture != nullptr)
				{
					m_vecTexture.push_back(pTexture);
					m_vecTextureUpload.push_back(pTextureUpload);
				}
				if (FAILED(Create_Shader_Resource_Heap()))
					return E_FAIL;
			}
		}
	}
	else if (eType == TEXTURE_TYPE_ELSE)
	{
		if (iNum == 0)
		{
			wsprintf(szFilePath, pFilepath, 0);

			BYTE* imageData;
			D3D12_RESOURCE_DESC textureDesc;
			int imageBytesPerRow;
			int imageSize = LoadImageDataFromFile(&imageData, textureDesc, szFilePath, imageBytesPerRow);

			D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

			if (imageSize <= 0)
				return E_FAIL;
			if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default,
				D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pTexture))))
				return E_FAIL;

			UINT64 textureUploadBufferSize;
			CDevice::GetInstance()->GetDevice()->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr,
				nullptr, nullptr, &textureUploadBufferSize);

			D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize);

			if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(
				&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
				&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&pTextureUpload))))
				return E_FAIL;

			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData = &imageData[0];
			textureData.RowPitch = imageBytesPerRow;
			textureData.SlicePitch = imageBytesPerRow * textureDesc.Height;

			UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pTexture, pTextureUpload, 0, 0, 1, &textureData);

			if (pTexture != nullptr)
			{
				pTexture->SetName(L"Texture");
				m_vecTexture.push_back(pTexture);
				pTextureUpload->SetName(L"Upload");
				m_vecTextureUpload.push_back(pTextureUpload);
			}
			if (FAILED(Create_Shader_Resource_Heap()))
				return E_FAIL;

			Safe_Delete(imageData);
		}
		else
		{
			for (_uint i = 0; i < iNum; ++i)
			{
				wsprintf(szFilePath, pFilepath, i);



			}
		}
	}
	else if (eType == TEXTURE_TGA)
	{
		if (iNum == 0)
		{
			wsprintf(szFilePath, pFilepath, 0);
			wstring wstrtemp = szFilePath;
			string strtemp = { wstrtemp.begin(), wstrtemp.end() };

			_uint	imageBytesPerRow = 0;
			_uint	iImageSize = 0;
			D3D12_RESOURCE_DESC textureDesc;

			FILE* pFile = nullptr;
			if (fopen_s(&pFile, strtemp.c_str(), "rb") != 0)
			{
				return E_FAIL;
			}
			_uint		iHeight = 0;
			_uint		iWidth = 0;


			_uint		iTempNum = 0;
			if (FAILED(LoadTargaDataFromFile(pFile, iImageSize, iHeight, iWidth, iTempNum)))
				return E_FAIL;
			//_ubyte* pTgaFile = new _ubyte[iImageSize]{};
			vector<_ubyte>	vectgaFile;
			vectgaFile.resize(iImageSize);
			if (FAILED(LoadTargaData(textureDesc, pFile, vectgaFile, iImageSize, iHeight, iWidth, imageBytesPerRow, iTempNum)))
				return E_FAIL;

			D3D12_HEAP_PROPERTIES	tHeap_Pro_Default = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_HEAP_PROPERTIES	tHeap_Pro_Upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

			if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Default,
				D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pTexture))))
				return E_FAIL;

			UINT64 textureUploadBufferSize;
			CDevice::GetInstance()->GetDevice()->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr,
				nullptr, nullptr, &textureUploadBufferSize);

			D3D12_RESOURCE_DESC		tResource_Desc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize);

			if (FAILED(CDevice::GetInstance()->GetDevice()->CreateCommittedResource(&tHeap_Pro_Upload, D3D12_HEAP_FLAG_NONE,
				&tResource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pTextureUpload))))
				return E_FAIL;

			D3D12_SUBRESOURCE_DATA textureData = {};
			//textureData.pData = &pTgaFile[0];
			textureData.pData = vectgaFile.data();
			textureData.RowPitch = imageBytesPerRow;
			textureData.SlicePitch = imageBytesPerRow * textureDesc.Height;

			UpdateSubresources(CDevice::GetInstance()->GetCommandList(), pTexture, pTextureUpload, 0, 0, 1, &textureData);

			if (pTexture != nullptr)
			{
				pTexture->SetName(L"Texture");
				m_vecTexture.push_back(pTexture);
				pTextureUpload->SetName(L"Upload");
				m_vecTextureUpload.push_back(pTextureUpload);
			}
			if (FAILED(Create_Shader_Resource_Heap()))
				return E_FAIL;

		}
		else
		{

		}
	}

	CDevice::GetInstance()->Close();
	CDevice::GetInstance()->WaitForGpuComplete();
	return S_OK;
}


CTexture* CTexture::Create(ID3D12Device* pGraphic_Device, const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType)
{
	CTexture* pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Ready_Texture(pFilepath, iNum, eType)))
	{
		MessageBox(0, L"CTexture Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}
HRESULT CTexture::Create_ShaderResourceView(_uint iNum, _bool IsCube)
{
	if (IsCube == false)
	{
		m_iTexuterIdx = iNum;
		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_vecDescriptorHeap[m_iTexuterIdx]->GetCPUDescriptorHandleForHeapStart());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_vecTexture[iNum]->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_vecTexture[iNum]->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_vecTexture[iNum], &srvDesc, hDescriptor);
	}
	else
	{

		m_iTexuterIdx = iNum;
		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_vecDescriptorHeap[m_iTexuterIdx]->GetCPUDescriptorHandleForHeapStart());

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_vecTexture[iNum]->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = m_vecTexture[iNum]->GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

		CDevice::GetInstance()->GetDevice()->CreateShaderResourceView(m_vecTexture[iNum], &srvDesc, hDescriptor);
	}

	return S_OK;
}
HRESULT CTexture::SetUp_OnShader(ID3D12GraphicsCommandList* pCommandLst, _int iIdx)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE hTexture
	(
		m_vecDescriptorHeap[iIdx]->GetGPUDescriptorHandleForHeapStart()
	);
	hTexture.Offset(0, m_vecSrvDescriptorIncrementSize[iIdx]);
	pCommandLst->SetDescriptorHeaps(1, &m_vecDescriptorHeap[iIdx]);
	pCommandLst->SetGraphicsRootDescriptorTable(0, hTexture);

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
	for (auto& iter : m_vecDescriptorHeap)
		Safe_Release(iter);
	if (m_IsClone)
	{

	}

	m_vecTexture.clear();
	m_vecTextureUpload.clear();
	m_vecDescriptorHeap.clear();

	m_vecTexture.shrink_to_fit();
	m_vecTextureUpload.shrink_to_fit();
	m_vecDescriptorHeap.shrink_to_fit();
	if (m_IsClone)
	{
		if (wicFactory)
			wicFactory->Release();
		if (wicDecoder)
			wicDecoder->Release();
		if (wicFrame)
			wicFrame->Release();
		if (wicConverter)
			wicConverter->Release();
	}
	CComponent::Free();
}

HRESULT CTexture::Create_Shader_Resource_Heap()
{
	ID3D12DescriptorHeap* pDescriptorHeap;
	_uint					iSize = 0;

	D3D12_DESCRIPTOR_HEAP_DESC DescHeap;
	ZeroMemory(&DescHeap, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	DescHeap.NumDescriptors = 1;
	DescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	DescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DescHeap.NodeMask = 0;

	if (FAILED(CDevice::GetInstance()->GetDevice()->CreateDescriptorHeap(&DescHeap, IID_PPV_ARGS(&pDescriptorHeap))))
		return E_FAIL;
	pDescriptorHeap->SetName(L"Disc");
	iSize = CDevice::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_vecDescriptorHeap.push_back(pDescriptorHeap);
	m_vecSrvDescriptorIncrementSize.push_back(iSize);

	return S_OK;
}

int CTexture::LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription,
	LPCWSTR filename, int& bytesPerRow)
{
	HRESULT hr;

	bool imageConverted = false;

	if (wicFactory == NULL)
	{
		CoInitialize(NULL);
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
		if (FAILED(hr)) return 0;
		hr = wicFactory->CreateFormatConverter(&wicConverter);
		if (FAILED(hr)) return 0;
	}

	hr = wicFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
	if (FAILED(hr)) return 0;

	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (FAILED(hr)) return 0;

	WICPixelFormatGUID pixelFormat;
	hr = wicFrame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)) return 0;

	UINT textureWidth, textureHeight;
	hr = wicFrame->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr)) return 0;

	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;
		dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

		BOOL canConvert = FALSE;
		hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (FAILED(hr) || !canConvert) return 0;

		hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (FAILED(hr)) return 0;

		imageConverted = true;
	}

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat);
	bytesPerRow = (textureWidth * bitsPerPixel) / 8;
	int imageSize = bytesPerRow * textureHeight;

	*imageData = (BYTE*)malloc(imageSize);

	if (imageConverted)
	{
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}
	else
	{
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (FAILED(hr)) return 0;
	}

	{
		resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDescription.Alignment = 0;
		resourceDescription.Width = textureWidth;
		resourceDescription.Height = textureHeight;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = dxgiFormat;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.SampleDesc.Quality = 0;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	return imageSize;
}

DXGI_FORMAT CTexture::GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

	else return DXGI_FORMAT_UNKNOWN;
}

WICPixelFormatGUID CTexture::GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

	else return GUID_WICPixelFormatDontCare;
}

int CTexture::GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
	else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

	else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
	else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
	else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
	else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
	else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;

	return 8;
}

HRESULT CTexture::LoadTargaDataFromFile(FILE* pFile, _uint& iImageSize, _uint& iHeight, _uint& iWidth, _uint& iTempNum)
{

	_uint		bpp;

	TargaFile	tTarga;

	_uint		iCnt = (_uint)fread(&tTarga, sizeof(TargaFile), 1, pFile);
	if (iCnt != 1)
		return E_FAIL;


	iHeight = (_uint)tTarga.iHeight;
	iWidth = (_uint)tTarga.iWidth;
	bpp = (_uint)tTarga.bpp;

	if (bpp == 32)
	{
		iTempNum = 4;
	}
	else if (bpp == 24)
	{
		iTempNum = 3;
	}
	else
		return E_FAIL;

	iImageSize = iWidth * iHeight * iTempNum;
	return S_OK;
}

HRESULT CTexture::LoadTargaData(D3D12_RESOURCE_DESC& tDesc, FILE* pFile, vector<_ubyte>& vecbyte,
	_uint iImageSize, _uint iHeight, _uint iWidth, _uint& iOutput, _uint iTempNum)
{
	vector<_ubyte>	vecTemp;
	vecTemp.resize(iImageSize);

	int iCnt = (_uint)fread(vecTemp.data(), 1, iImageSize, pFile);
	if (iCnt != iImageSize)
		return E_FAIL;

	if (fclose(pFile) != 0)
		return E_FAIL;

	_uint iIdx = 0;
	_uint k = (iWidth * iHeight * iTempNum) - (iWidth * iTempNum);
	int bitsPerPixel = 0;

	if (iTempNum == 3)
	{
		for (_uint j = 0; j < iHeight; ++j)
		{
			for (_uint i = 0; i < iWidth; ++i)
			{
				vecbyte[iIdx + 0] = vecTemp[k + 2];
				vecbyte[iIdx + 1] = vecTemp[k + 1];
				vecbyte[iIdx + 2] = vecTemp[k + 0];

				k += 3;
				iIdx += 3;
			}

			k -= (iWidth * 6);
		}


		bitsPerPixel = 24;
		iOutput = (iWidth * bitsPerPixel) / 6;


		tDesc.Height = iHeight;
		tDesc.Width = iWidth;
		tDesc.MipLevels = 1;
		tDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		tDesc.Alignment = 0;
		tDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		tDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		tDesc.DepthOrArraySize = 1;

	}
	else if (iTempNum == 4)
	{
		for (_uint j = 0; j < iHeight; ++j)
		{
			for (_uint i = 0; i < iWidth; ++i)
			{
				vecbyte[iIdx + 0] = vecTemp[k + 2];
				vecbyte[iIdx + 1] = vecTemp[k + 1];
				vecbyte[iIdx + 2] = vecTemp[k + 0];
				vecbyte[iIdx + 3] = vecTemp[k + 3];

				k += 4;
				iIdx += 4;
			}

			k -= (iWidth * 8);
		}


		bitsPerPixel = 32;
		iOutput = (iWidth * bitsPerPixel) / 8;

		tDesc.Height = iHeight;
		tDesc.Width = iWidth;
		tDesc.MipLevels = 1;
		tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tDesc.SampleDesc.Count = 1;
		tDesc.SampleDesc.Quality = 0;
		tDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		tDesc.Alignment = 0;
		tDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		tDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		tDesc.DepthOrArraySize = 1;
	}





	return S_OK;
}
