#pragma once
#include "Component.h"
class CTexture :
    public CComponent
{
private:
    CTexture(ID3D12Device* pGraphic_Device);
    CTexture(const CTexture& rhs);
    ~CTexture() = default;
public:
    HRESULT                         Ready_Texture(const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType = TEXTURE_TYPE_ELSE);
public:
    static CTexture*                Create(ID3D12Device* pGraphic_Device, const _tchar* pFilepath, _uint iNum=1, 
                                            TEXTURE_TYPE eType = TEXTURE_TYPE_ELSE);
    virtual CComponent*             Clone_Component(void* pArg = nullptr);
public:
    HRESULT                         Create_ShaderResourceView(_uint iNum=0);
    HRESULT                         SetUp_OnShader(ID3D12GraphicsCommandList* pCommandLst);
protected:
    virtual void					Free();
private:
    HRESULT                         Create_Shader_Resource_Heap();
private:
    int                             LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, 
                                                            LPCWSTR filename, int& bytesPerRow);
    DXGI_FORMAT                     GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
    WICPixelFormatGUID              GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
    int                             GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
private:
    TEXTURE_TYPE                    m_eType = TEXTURE_TYPE_END;
    _uint                           m_iTexuterIdx = 0;
private:
    vector<ID3D12Resource*>         m_vecTexture;
    vector<ID3D12Resource*>         m_vecTextureUpload;
private:
    vector<ID3D12DescriptorHeap*>   m_vecDescriptorHeap;
    vector<_uint>                   m_vecSrvDescriptorIncrementSize;
    vector<BYTE*>                   m_vecImageData;
private:
    ID3D12DescriptorHeap*           m_pTempDecriptor;
private:
    _bool                           m_IsClone = false;
private:
    IWICImagingFactory*             wicFactory = NULL;
    IWICBitmapDecoder*              wicDecoder = NULL;
    IWICBitmapFrameDecode*          wicFrame = NULL;
    IWICFormatConverter*            wicConverter = NULL;
};

