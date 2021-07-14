#pragma once
#include "GameObject.h"
#include "Input.h"
#include "Transform.h"

class CCamera :
    public CGameObject
{
protected:
	explicit CCamera( );
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;
public:
	virtual HRESULT				Ready_Prototype();
	virtual HRESULT				Ready_GameObject(void* pArg = nullptr);
	virtual _int				Update_GameObject(const _float & fTimeDelta);
	virtual _int				LastUpdate_GameObject(const _float & fTimeDelta);
	virtual void				Render_GameObject();
public:
	void						ShadowMatView();
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC & CameraDesc, const PROJDESC & ProjDesc);
	void						Invalidate_ViewProjMatrix();
public:
	HRESULT						SetUp_CameraProjDesc(const CAMERADESC& CameraDesc, const PROJDESC& ProjDesc, _bool IsShadow);
	HRESULT						SetUp_ViewProjMatrices(_bool IsShadow);
	HRESULT						Once_SetUp_ViewProjMatrices(_bool IsShadow, _vec3 vPos);
	void						Invalidate_ViewProjMatrix(_bool IsShadow);
private:
	HRESULT						SetUp_ViewProjMatrices();
public:
	virtual CGameObject*		Clone_GameObject(void* pArg = nullptr, _uint iIdx = 0) PURE;
protected:
	virtual void				Free();
protected:
	CInput*						m_pInput_Device = nullptr;
protected:
	CTransform*					m_pTransform = nullptr;

	_matrix						m_matView;
	CAMERADESC					m_tCameraDesc;
protected:
	_matrix						m_matProj;
	PROJDESC					m_tProjDesc;
protected:
	_matrix						m_matShadowView;
	CAMERADESC					m_tShadowCameraDesc;
protected:
	_matrix						m_matShadowProj;
	PROJDESC					m_tShadowProjDesc;
protected:
	_vec3						m_vRight, m_vUp, m_vLook, m_vPos; 

private:
	bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon())
	{
		return (epsilon > std::abs(a - b));
	}

	_vec3 DecomposeRotMat(const Matrix& _matRot)
	{
		// _mat 을 분해 후 다시 행렬 만들기	
		_vec4 vMat[4];
		XMStoreFloat4(&vMat[0], _matRot.Right());   // 1
		XMStoreFloat4(&vMat[1], _matRot.Up());      // 2
		XMStoreFloat4(&vMat[2], _matRot.Forward());   // 3
		XMStoreFloat4(&vMat[3], _matRot.Translation());//4

		_vec3 vNewRot;
		if (closeEnough(vMat[0].z, -1.0f))
		{
			float x = 0; //gimbal lock, value of x doesn't matter
			float y = XM_PI / 2;
			float z = x + atan2(vMat[1].x, vMat[2].x);
			vNewRot = _vec3{ x, y, z };
		}
		else if (closeEnough(vMat[0].z, 1.0f)) {
			float x = 0;
			float y = -XM_PI / 2;
			float z = -x + atan2(-vMat[1].x, -vMat[2].x);
			vNewRot = _vec3{ x, y, z };
		}
		else { //two solutions exist
			float y1 = -asin(vMat[0].z);
			float y2 = XM_PI - y1;

			float x1 = atan2f(vMat[1].z / cos(y1), vMat[2].z / cos(y1));
			float x2 = atan2f(vMat[1].z / cos(y2), vMat[2].z / cos(y2));

			float z1 = atan2f(vMat[0].y / cos(y1), vMat[0].x / cos(y1));
			float z2 = atan2f(vMat[0].y / cos(y2), vMat[0].x / cos(y2));

			//choose one solution to return
			//for example the "shortest" rotation
			if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
			{
				vNewRot = _vec3{ x1, y1, z1 };
			}
			else {
				vNewRot = _vec3{ x2, y2, z2 };
			}
		}
		return vNewRot;
	}
};

