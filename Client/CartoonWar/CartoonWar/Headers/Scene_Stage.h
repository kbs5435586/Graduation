#pragma once
#include "Scene.h"
class CManagement;
class CScene_Stage :
    public CScene
{
private:
    CScene_Stage();
    virtual ~CScene_Stage() = default;
public:
	virtual HRESULT			Ready_Scene() override;
	virtual _int			Update_Scene(const _float & fTimeDelta) override;
	virtual _int			LastUpdate_Scene(const _float & fTimeDelta) override;
	virtual void			Render_Scene() override;
private:
	HRESULT					Ready_Prototype_GameObject(CManagement * pManagement);
	HRESULT					Ready_Layer(CManagement * pManagement);
	HRESULT					Ready_Light(CManagement * pManagement);
private:
	HRESULT					Ready_Layer_BasicShape(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Debug_Camera(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Light_Camera(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Inventory_Camera(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Reflection_Camera(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Map_Camera(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_SkyBox(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Terrain(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Terrain_Height(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_UI_Select(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Deffered_UI(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Environment(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_SkillFire(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Range(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Particle(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Flag(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Test(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Map(const _tchar* pLayerTag, CManagement* pManagement);
private:
	HRESULT					Ready_Layer_Bloom(const _tchar* pLayerTag, CManagement* pManagement);
private:
	HRESULT					Ready_Layer_Player(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Inventory(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_NPC(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Animals(const _tchar* pLayerTag, CManagement* pManagement);
public:
	static CScene_Stage*	Create();
protected:
	virtual void			Free();
};

