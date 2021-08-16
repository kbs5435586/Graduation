#include "framework.h"
#include "FontMgr.h"
#include "Management.h"
#include "StructedBuffer.h"

#include "Font.h"

_IMPLEMENT_SINGLETON(CFontMgr)
CFontMgr::CFontMgr()
{

}

HRESULT CFontMgr::Ready_FontMgr(const char* pFilePath)
{

	ifstream fin;
	fin.open(pFilePath);
	if (fin.fail())
	{
		return E_FAIL;
	}
	for (int i = 0; i < 95; i++)
	{
		FontType pFont;
		char temp = 0;
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

		fin >> pFont.left;
		fin >> pFont.right;
		fin >> pFont.size;

		m_vecFontData.push_back(pFont);
	}

	fin.close();



	m_pShaderCom = CShader::Create(L"../ShaderFiles/Shader_Font.hlsl", "VS_Main", "PS_Main");
	m_pTextureCom = CTexture::Create(L"../Bin/Resource/Texture/Font/font%d.dds", 1, TEXTURE_TYPE::TEXTURE_TYPE_DDS);
	m_pTransformCom = CTransform::Create();
	if (FAILED(CreateInputLayout()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFontMgr::Create_Buffer(const _tchar* pFontTag,const char* sentence, float drawX, float drawY)
{
	CFont* pFont = CFont::Create(sentence, drawX, drawY, m_vecFontData);
	if (nullptr == pFont)
		return E_FAIL;
	//m_vecFont.push_back(pFont);
	m_mapFont.insert({ pFontTag , pFont});

	return S_OK;
}

void CFontMgr::Update_Font()
{
	//if (nullptr != m_pRendererCom)
	{
		//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		//	return ;
	}
}

void CFontMgr::Render_Font()
{
	for (auto& iter : m_mapFont)
		iter.second->Render_Font(m_pShaderCom, m_pTextureCom);
}

void CFontMgr::Delete_Font(const _tchar* pFontTag)
{

	auto iter_find = find_if(m_mapFont.begin(), m_mapFont.end(), CFinder_Tag(pFontTag));
	if (iter_find == m_mapFont.end())
		return;

	auto iter = m_mapFont.begin();
	for (; iter != m_mapFont.end();)
	{
		if (!lstrcmp(iter->first, pFontTag))
		{
			Safe_Release(iter->second);
			iter = m_mapFont.erase(iter);
		}
		else iter++;
	}
}

void CFontMgr::Delete_All_Fo1nt()
{
	auto iter = m_mapFont.begin();
	for (; iter != m_mapFont.end();)
	{
		if (iter->first)
		{
			Safe_Release(iter->second);
			iter = m_mapFont.erase(iter);
		}
		else iter++;
	}
}

HRESULT CFontMgr::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	vector<D3D12_INPUT_ELEMENT_DESC>  vecDesc;
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	vecDesc.push_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	//LESS_EQUAL
	if (FAILED(m_pShaderCom->Create_Shader(vecDesc, RS_TYPE::DEFAULT, DEPTH_STENCIL_TYPE::LESS_EQUAL, SHADER_TYPE::SHADER_FORWARD, BLEND_TYPE::ALPHABLEND)))
		return E_FAIL;

	return S_OK;
}

CFontMgr* CFontMgr::Create(const char* pFilePath)
{
	CFontMgr* pInstance = new CFontMgr();
	if (FAILED(pInstance->Ready_FontMgr(pFilePath)))
		Safe_Release(pInstance);
	return pInstance;
}

void CFontMgr::Free()
{
	for (auto& iter : m_mapFont)
	{
		Safe_Release(iter.second);
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
