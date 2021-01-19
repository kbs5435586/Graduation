#pragma once
#include "Component.h"
#include "DirectXTex.h"
#include "DirectXTex.inl"

class CTexture :
    public CComponent
{
private:
    struct TargaFile
    {
        _ubyte		data1[12];
        _ushort		iWidth;
        _ushort		iHeight;
        _ubyte		bpp;
        _ubyte		data2;

    };
private:
    CTexture();
    CTexture(const CTexture& rhs);
    ~CTexture() = default;
public:
    HRESULT                         Ready_Texture(const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube);
    static CTexture* Create(const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube = false);
    virtual CComponent* Clone_Component(void* pArg = nullptr);
    HRESULT                         SetUp_OnShader(_int iIdx = 0);
private:
    HRESULT                         Create_ShaderResourceView(ScratchImage& Image, _bool IsCube);
protected:
    virtual void					Free();
private:
    TEXTURE_TYPE                    m_eType = TEXTURE_TYPE::TEXTURE_TYPE_END;
    _uint                           m_iTexuterIdx = 0;
private:
    vector<ID3D12Resource*>         m_vecTexture;
    vector<ID3D12Resource*>         m_vecTextureUpload;
    vector<ID3D12DescriptorHeap*>   m_vecDescriptorHeap;
    vector<_uint>                   m_vecSrvDescriptorIncrementSize;

private:
    _bool                           m_IsClone = false;
private:
    ScratchImage			 	    m_Image;

};

