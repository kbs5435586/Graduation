#pragma once
#include "Component.h"
class CShader;
class CTexture;
class CFont :
	public CBase
{
private:
	CFont();
	virtual ~CFont() = default;
public:
	HRESULT						Create_Buffer(const char* pSentence, _float fDrawX, _float fDrawY, vector<FontType> vecFontInfo);
	void						Render_Font(CShader* pShader, CTexture* pTexture);
private:
	vector<FontInfo>			m_vecFontInfo;

private:
	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_uint						m_iSlot = 0;
	_uint						m_iStride = 0;
	_uint						m_iOffset = 0;
	_uint						m_iNumVertices = 0;
public:
	static						CFont* Create(const char* pSentence, _float fDrawX, _float fDrawY, vector<FontType> vecFontInfo);
private:
	virtual void				Free();

};