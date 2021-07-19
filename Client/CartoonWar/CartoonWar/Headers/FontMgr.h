#pragma once
#include "Base.h"
class CFont;
class CTransform;
class CShader;
class CTexture;
class CRenderer;
class CFontMgr :
    public CBase
{
    _DECLARE_SINGLETON(CFontMgr)
private:
    CFontMgr();
    virtual ~CFontMgr() = default;
public:
    HRESULT						Ready_FontMgr(const char* pFilePath);
	HRESULT						Create_Buffer(const _tchar* pFontTag, const char* sentence, float drawX, float drawY);
    void                        Update_Font();
	void						Render_Font();
    void                        Delete_Font(const _tchar* pFontTag);
    void                        Delete_All_Fo1nt();
private:
    vector<FontType>			m_vecFontData;
    map<const _tchar*, CFont*>  m_mapFont;
private:
    CTransform*                 m_pTransformCom = nullptr;
    CShader*                    m_pShaderCom = nullptr;
    CTexture*                   m_pTextureCom = nullptr;
private:
	HRESULT						CreateInputLayout();
public:
    static CFontMgr*			Create(const char* pFilePath);
private:
    virtual void				Free();
};

