#pragma once
#include "Component.h"
#include "DirectXTex.h"
#include "DirectXTex.inl"

class CTexture :
    public CComponent
{
private:
    CTexture();
    CTexture(const CTexture& rhs);
    ~CTexture() = default;
public:
    HRESULT                                             Ready_Texture(const _tchar* pFilePath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube);
    HRESULT                                             Ready_Texture(const _tchar* pTag, const _tchar* pFilePath);
    HRESULT                                             Ready_Texture(const _tchar* pFilePath);
public:
    static CTexture*                                    Create(const _tchar* pFilePath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube = false);
    static CTexture*                                    Create(const _tchar* pTag, const _tchar* pFilePath);
    static CTexture*                                    Create(const _tchar* pFilePath);
public:
    virtual CComponent*                                 Clone_Component(void* pArg = nullptr);
    HRESULT                                             SetUp_OnShader(_int iIdx = 0, TEXTURE_REGISTER eRegister = TEXTURE_REGISTER::t0);
private:
    HRESULT                                             Create_ShaderResourceView(ScratchImage& Image, _bool IsCube = false);
    HRESULT                                             Create_ShaderResourceView(ScratchImage& Image, vector<ID3D12DescriptorHeap*>& pDesc);
    HRESULT                                             Create_ShaderResourceView_(ScratchImage& Image, ComPtr<ID3D12DescriptorHeap>& pSrv);
protected:
    virtual void					                    Free();
private:
    TEXTURE_TYPE                                        m_eType = TEXTURE_TYPE::TEXTURE_TYPE_END;
    _uint                                               m_iTexuterIdx = 0;
public:
   ID3D12DescriptorHeap*                                GetSRV(const _uint& iTextureIdx=0) { return m_vecSRV[iTextureIdx]; }
   ComPtr<ID3D12DescriptorHeap>                         GetSRV_() { return m_pSRV; }
   ID3D12DescriptorHeap*                                GetUAV(const _uint& iIdx=0){return m_vecUAV[iIdx]; }
   ID3D12Resource*                                      GetTexture(const _uint& iTextureID=0){return m_vecTexture[iTextureID];}
public:
    D3D12_RESOURCE_STATES                               GetState() { return m_eState; }
    void                                                SetState(D3D12_RESOURCE_STATES eState) { m_eState = eState; }
public:
    _vec2                                               GetTextureResolution() { return m_vTextureResolution; }
private:
    vector<ID3D12Resource*>                             m_vecTexture;
    vector<ID3D12Resource*>                             m_vecTextureUpload;
    vector<ID3D12DescriptorHeap*>                       m_vecSRV;
    vector<ID3D12DescriptorHeap*>                       m_vecUAV;
    vector<_uint>                                       m_vecSrvDescriptorIncrementSize;
    ComPtr<ID3D12DescriptorHeap>                        m_pSRV = nullptr;
private:
    D3D12_RESOURCE_STATES                               m_eState = {};
private:
    map<const _tchar*, vector<ID3D12DescriptorHeap*>>   m_mapSrvDescHeap;
private:
    vector<ID3D12DescriptorHeap*>                       m_vecDummyTextureDesc;
    ID3D12DescriptorHeap*                               m_pInitTextureDesc = nullptr;
private:
    ScratchImage			 	                        m_Image;
private:
    _vec2                                               m_vTextureResolution = {};

};

