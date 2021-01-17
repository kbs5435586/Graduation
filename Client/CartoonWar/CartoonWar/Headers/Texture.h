#pragma once
#include "Component.h"
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
public:
    static CTexture*                Create(const _tchar* pFilepath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube = false);
    virtual CComponent*             Clone_Component(void* pArg = nullptr);
public:
    HRESULT                         Create_ShaderResourceView(_uint iNum=0, _bool IsCube = false);
    HRESULT                         SetUp_OnShader(_int iIdx=0);
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
    HRESULT                         LoadTargaDataFromFile(FILE* pFile, _uint& iImageSize, _uint& iHeight, _uint& iWidth, _uint& iTempNum);
    HRESULT                         LoadTargaData(D3D12_RESOURCE_DESC& tDesc, FILE* pFile, vector<_ubyte>& vecbyte, 
                                        _uint iImageSize, _uint iHeight, _uint iWidth, _uint& iOutput, _uint iTempNum);
private:
    TEXTURE_TYPE                    m_eType = TEXTURE_TYPE::TEXTURE_TYPE_END;
    _uint                           m_iTexuterIdx = 0;
private:
    vector<ID3D12Resource*>         m_vecTexture;
    vector<ID3D12Resource*>         m_vecTextureUpload;
private:
    vector<ID3D12DescriptorHeap*>   m_vecDescriptorHeap;
    vector<_uint>                   m_vecSrvDescriptorIncrementSize;
    vector<BYTE*>                   m_vecImageData;
private:
    _bool                           m_IsClone = false;
private:
    IWICImagingFactory*             wicFactory = NULL;
    IWICBitmapDecoder*              wicDecoder = NULL;
    IWICBitmapFrameDecode*          wicFrame = NULL;
    IWICFormatConverter*            wicConverter = NULL;

};

