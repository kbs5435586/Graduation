#pragma once
#include "GameObject.h"
class CTransform;
class CShader;
class CRenderer;
class CStatic_Mesh;
class CTexture;
class CFrustum;

class CTempStaticMesh :
    public CGameObject
{
private:
    CTempStaticMesh(ID3D12Device* pGraphic_Device);
    CTempStaticMesh(const CTempStaticMesh& rhs);
    virtual ~CTempStaticMesh() = default;
public:
	virtual HRESULT							Ready_Prototype();
	virtual HRESULT							Ready_GameObject(void* pArg = nullptr);
	virtual _int							Update_GameObject(const _float& fTimeDelta);
	virtual _int							LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void							Render_GameObject();
public:
	virtual HRESULT							CreatePipeLine(CShader* pShader);
	virtual D3D12_RASTERIZER_DESC			CreateRaterizerState();
	virtual D3D12_DEPTH_STENCIL_DESC		CreateDepthStencilState();
	virtual D3D12_BLEND_DESC				CreateBlendState();
	virtual D3D12_INPUT_LAYOUT_DESC			CreateInputLayout();
private:
	HRESULT									Ready_Light(MAINPASS_LIGHT& tInfo);
private:
	HRESULT									CreateConstantBuffer();
public:
	static CTempStaticMesh*					Create(ID3D12Device* pGraphic_Device);
	virtual CGameObject*					Clone_GameObject(void* pArg) override;
private:
	virtual void							Free();
	HRESULT									Ready_Component();
private:
	CTransform*								m_pTransformCom = nullptr;
	CShader*								m_pShaderCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CStatic_Mesh*							m_pMeshCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CFrustum*								m_pFrustumCom = nullptr;
};
