#pragma once
#include "Scene.h"
class CManagement;
class CScene_Logo :
    public CScene
{
private:
	explicit CScene_Logo(ID3D12Device* pGraphic_Device);
	virtual ~CScene_Logo() = default;
public:
	virtual HRESULT			Ready_Scene() override;
	virtual _int			Update_Scene(const _float & fTimeDelta) override;
	virtual _int			LastUpdate_Scene(const _float & fTimeDelta) override;
	virtual void			Render_Scene() override;
private:
	HRESULT					Ready_Prototype_GameObject();
	HRESULT					Ready_Prototype_Component();
	HRESULT					Ready_Light();
private:
	HRESULT					Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_Cube(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_Rect(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_TextureRect(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_Terrain_Texture(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_Terrain_Height(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_Temp_Mesh(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_TempStatic_Mesh(const _tchar* pLayerTag);
	HRESULT					Ready_Layer_SkyBox(const _tchar* pLayerTag);

private:
	HRESULT					Add_Prototype_Component_Shader(CManagement* pManagement);
	HRESULT					Add_Prototype_Component_Buffer(CManagement* pManagement);
	HRESULT					Add_Prototype_Component_Texture(CManagement* pManagement);
	HRESULT					Add_Prototype_Component_Static_Mesh(CManagement* pManagement);
	HRESULT					Add_Prototype_Component_Dynamic_Mesh(CManagement* pManagemet);
public:
	static CScene_Logo*		Create(ID3D12Device* pGraphic_Device);
protected:
	virtual void			Free();
private:
	_float					m_fTempTime = 0.f;
};

