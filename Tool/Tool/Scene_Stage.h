#pragma once
#include "Scene.h"
class CScene_Stage :
	public CScene
{
private:
	explicit CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage() = default;
public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual _int LastUpdate_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene() override;
private:
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();
private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT	Ready_Layer_Stone(const _tchar* pLayerTag);
private:
	HRESULT Ready_LightInfo();
public:
	static CScene_Stage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free();
};

