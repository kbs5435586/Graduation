#pragma once
#include "Scene.h"
class CTool_Scene :
	public CScene
{
private:
	CTool_Scene(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CTool_Scene()=default;
public:
	virtual HRESULT Ready_Scene() override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual _int LastUpdate_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene() override;
private:
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();
private:
	HRESULT Ready_LightInfo();
private:
	HRESULT Ready_Camera_Layer(const _tchar* pLayerTag);
	HRESULT Ready_Terrain_Layer(const _tchar* pLayerTag);
	HRESULT	Ready_SkyBox_Layer(const _tchar* pLayerTag);
	HRESULT Ready_Player_Layer(const _tchar* pLayerTag);
public:
	static CTool_Scene* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free();
};
