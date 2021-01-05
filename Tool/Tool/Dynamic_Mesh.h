#pragma once
#include "Component.h"
class CHierarchyLoader;
class CAnimationCtrl;
class CDynamic_Mesh :
	public CComponent
{
private:
	CDynamic_Mesh(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDynamic_Mesh(const CDynamic_Mesh& rhs);
	virtual ~CDynamic_Mesh()=default;
public:
	_uint Get_NumMeshContainer() const {return m_vecMeshContainer.size();}

	const D3DXMESHCONTAINER_DERIVED* Get_MeshContainer(const _uint& iIndex) const {return m_vecMeshContainer[iIndex];}

public:
	HRESULT Ready_Mesh_Dynamic(const _tchar* pFilePath, const _tchar* pFileName);
	void	Update_Skinning(const _uint& iMeshContainerIdx);
	void	Render_Mesh(const _uint& iMeshContainerIdx, const _uint& iAttributeID);
	HRESULT Set_AnimationSet(const _uint& iAnimationID, ANIMATE_PLAY eID);
	HRESULT Set_AnimationSet(const _uint& iAnimationID);
	void	Play_Animation(const _float& fTimeDelta);
	_bool	Get_EndLoop();
public:
	_uint	Get_CurrentState();
private:
	CHierarchyLoader*	m_pLoader = nullptr;
	D3DXFRAME*			m_pRootFrame = nullptr;
	_matrix				m_matPivot;
	CAnimationCtrl*		m_pAnimationCtrl = nullptr;
private:
	vector<D3DXMESHCONTAINER_DERIVED*>			m_vecMeshContainer;
	typedef vector<D3DXMESHCONTAINER_DERIVED*>	VECMESHCONTAINER;
public:
	void Update_CombinedTransformationMatrices(D3DXFRAME_DERIVED*	pFrame, const _matrix* pParentMatrix);
	HRESULT SetUp_CombinedMatrixPointer(D3DXFRAME_DERIVED*	pFrame);
public:
	static CDynamic_Mesh* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();

};

