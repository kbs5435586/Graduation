#pragma once
#include "Base.h"
class CTexture;
class CShader;
class CBuffer_RectTex;
class CFont :
	public CBase
{
	_DECLARE_SINGLETON(CFont)
	struct FontType
	{
		_float	fLeft;
		_float	fRight;
		_int	iSize;;
	};

private:
	CFont();
	virtual ~CFont() = default;
public:
	HRESULT						Ready_Font(const _tchar* pFontTex, const char* pFontData);
	HRESULT						Load_Font_Data(const char* pFontData);
	HRESULT						Load_Font_Tex(const _tchar* pFontTex);
public:
	HRESULT						Initialize_Sentence(SENTENCE* pSentence, _uint iMaxLenght);
	HRESULT						Update_Sentence(SENTENCE* pSentence, char* pMessage, _float fPositionX, _float fPositionY);
public:
	CTexture*					GetTexture(){return m_pTextureCom;}
	CShader*					GetShader(){return m_pShaderCom;}
private:
	CTexture*					m_pTextureCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
private:
	vector<FontType*>			m_vecFont;
private:
	SENTENCE*					m_pSentence=nullptr;
private:
	ComPtr<ID3D12Resource>		m_pVertexBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pVertexUploadBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pIndexBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pIndexUploadBuffer = nullptr;
private:
	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_uint						m_iSlot = 0;
	_uint						m_iStride = 0;
	_uint						m_iOffset = 0;
private:
	_uint						m_iNumVertices = 0;
	_uint						m_iNumIndices = 0;
private:
	D3D12_VERTEX_BUFFER_VIEW	m_tVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW		m_tIndexBufferView;
public:
	static CFont*				Create(const _tchar* pFontTex, const char* pFontData);
private:
	virtual void				Free();
};

