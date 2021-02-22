#pragma once
#include "Component.h"
#include "DirectXTex.h"
#include "../Codes/DirectXTex.inl"
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
public:
    static CTexture*                                    Create(const _tchar* pFilePath, _uint iNum, TEXTURE_TYPE eType, _bool IsCube = false);
    static CTexture*                                    Create(const _tchar* pTag, const _tchar* pFilePath);
public:
    virtual CComponent*                                 Clone_Component(void* pArg = nullptr);
    HRESULT                                             SetUp_OnShader(_int iIdx = 0, TEXTURE_REGISTER eRegister = TEXTURE_REGISTER::t0);
private:
    HRESULT                                             Create_ShaderResourceView(ScratchImage& Image, _bool IsCube = false);
    HRESULT                                             Create_ShaderResourceView(ScratchImage& Image, vector<ID3D12DescriptorHeap*>& pDesc);
protected:
    virtual void					                    Free();
private:
    TEXTURE_TYPE                                        m_eType = TEXTURE_TYPE::TEXTURE_TYPE_END;
    _uint                                               m_iTexuterIdx = 0;
public:
   ID3D12DescriptorHeap*                                GetSRV(const _uint& iTextureIdx=0) { return m_vecDescriptorHeap[iTextureIdx]; }
   ID3D12DescriptorHeap*                                GetSRV(const _tchar* pTag, const _uint& iTextureIdx);
private:
    vector<ID3D12Resource*>                             m_vecTexture;
    vector<ID3D12Resource*>                             m_vecTextureUpload;
    vector<ID3D12DescriptorHeap*>                       m_vecDescriptorHeap;
    vector<_uint>                                       m_vecSrvDescriptorIncrementSize;
private:
    map<const _tchar*, vector<ID3D12DescriptorHeap*>>   m_mapSrvDescHeap;
private:
    vector<ID3D12DescriptorHeap*>                       m_vecDummyTextureDesc;
    ID3D12DescriptorHeap*                               m_pInitTextureDesc = nullptr;

private:
    _bool                                               m_IsClone = false;
private:
    ScratchImage			 	                        m_Image;

};

