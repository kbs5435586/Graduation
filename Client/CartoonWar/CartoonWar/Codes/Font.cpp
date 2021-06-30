#include "framework.h"
#include "Font.h"
#include "Management.h"

_IMPLEMENT_SINGLETON(CFont)

CFont::CFont()
{
}

HRESULT CFont::Ready_Font(const _tchar* pFontTex, const char* pFontData)
{	
	if (FAILED(Load_Font_Data(pFontData)))
		return E_FAIL;
	if (FAILED(Load_Font_Tex(pFontTex)))
		return E_FAIL;
	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Font.hlsl", "VS_Main", "PS_Main");
	if (nullptr == m_pShaderCom)
		return E_FAIL;


	{

		m_iNumVertices = 4;
		m_iStride = sizeof(VTXTEX);
		m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		vector<VTXTEX>		vecVertices;
		vecVertices.resize(m_iNumVertices);

		vecVertices[0] = VTXTEX(XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT2(0.0f, 0.0f));
		vecVertices[1] = VTXTEX(XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT2(1.0f, 0.0f));
		vecVertices[2] = VTXTEX(XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT2(1.f, 1.f));
		vecVertices[3] = VTXTEX(XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT2(0.0f, 1.f));


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
	}


	return S_OK;
}

HRESULT CFont::Load_Font_Data(const char* pFontData)
{
	ifstream fin;
	int i=0;
	char temp=0;


	

	fin.open(pFontData);
	if (fin.fail())
		return E_FAIL;


	for (int i = 0; i < 95; ++i)
	{
		FontType* pFont = new FontType;

		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);

		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin >> pFont->fLeft;
		fin >> pFont->fRight;
		fin >> pFont->iSize;
		m_vecFont.push_back(pFont);
	}
	fin.close();


	return S_OK;
}

HRESULT CFont::Load_Font_Tex(const _tchar* pFontTex)
{
	if (nullptr != m_pTextureCom)
		return E_FAIL;


	m_pTextureCom = CTexture::Create(pFontTex, 1, TEXTURE_TYPE::TEXTURE_TYPE_PNG_JPG);
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	
	return S_OK;
}

HRESULT CFont::Initialize_Sentence(SENTENCE* pSentence, _uint iMaxLenght)
{
	pSentence = new SENTENCE;
	pSentence->iMaxLenght = iMaxLenght;
	pSentence->iVertexCnt = 4* iMaxLenght;

	return S_OK;
}

HRESULT CFont::Update_Sentence(SENTENCE* pSentence, char* pMessage, _float fPositionX, _float fPositionY)
{

	return S_OK;
}

CFont* CFont::Create(const _tchar* pFontTex, const char* pFontData)
{
	CFont* pInstance = new CFont();
	if (FAILED(pInstance->Ready_Font(pFontTex, pFontData)))
		Safe_Release(pInstance);

	return pInstance;
}

void CFont::Free()
{
	for (auto& iter : m_vecFont)
		Safe_Delete(iter);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
