#pragma once
#include "Base.h"
class CShader;
class CTexture;
class CStructedBuffer;
class CTransform;
class CFontMgr :
    public CBase
{
    _DECLARE_SINGLETON(CFontMgr)
private:
    CFontMgr();
    virtual ~CFontMgr() = default;
public:
    HRESULT						Ready_FontMgr(const char* pFilePath);
	HRESULT						Create_Buffer(const char* sentence, float drawX, float drawY);
	void						Render_Font();
private:
    vector<FontType>			m_vecFont;
    CStructedBuffer*			m_pFontOffset;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
private:
	ComPtr<ID3D12Resource>		m_pVertexBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pVertexUploadBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pIndexBuffer = nullptr;
	ComPtr<ID3D12Resource>		m_pIndexUploadBuffer = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_uint						m_iSlot = 0;
	_uint						m_iStride = 0;
	_uint						m_iOffset = 0;
private:
	vector<FontInfo>			m_vecFontInfo;
	_uint						m_iNumVertices = 0;
private:
	HRESULT						CreateInputLayout();
public:
    CStructedBuffer*			GetFontOffset(){return m_pFontOffset;}
public:
    static CFontMgr*			Create(const char* pFilePath);
private:
    virtual void				Free();
};

