#pragma once
#include "Base.h"
class CShader;
class CTransform;
class CBuffer_RectTex;
class CRenderer;
class CDebug_Camera;
class CLight :
	public CBase
{
private:
	CLight();
	~CLight() = default;
public:
	HRESULT						Ready_Light(LIGHT & tLightInfo);
	void						Update();
	void						Render();
private:
	HRESULT						CreateInputLayout(_uint iType);
public:
	LIGHT&						Get_LightInfo() { return m_tLight; }
	void						Set_ArrIdx(_uint i) { m_iArridx = i; }
	_uint						Get_ArrIdx() { return m_iArridx; }
private:
	void						Set_LookAt(const _vec3& vLook);
private:
	LIGHT						m_tLight = {};
public:
	static CLight*				Create(LIGHT & tLightInfo);
private:
	virtual void				Free();
private:
	CTransform*					m_pTransformCom = nullptr;
	CShader*					m_pShader_Dir = nullptr;
	CShader*					m_pShader_Point = nullptr;
	CShader*					m_pShader_Merge = nullptr;
	CBuffer_RectTex*			m_pBufferCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	_uint						m_iArridx = 0;
private:
	CDebug_Camera*				m_pGameObject = nullptr;
private:
	_vec3						m_vLocalPos;	// GameObject 좌표
	_vec3						m_vLocalScale;	// GameObject 크기
	_vec3						m_vLocalRot;	// GameObject 회전량
	_vec3						m_vLocalDir[3]; // GameObject 의 Front, Up, Right 방향
	_vec3						m_vWorldDir[3]; // GameObject 의 Front, Up, Right 방향

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

