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
	HRESULT					Ready_Layer_SkyBox(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Terrain(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Terrain_Height(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Orc(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_UI(const _tchar* pLayerTag, CManagement* pManagement);
	HRESULT					Ready_Layer_Test(const _tchar* pLayerTag, CManagement* pManagement);
public:
	static CScene_Stage*	Create();
protected:
	virtual void			Free();
};

