#pragma once

class CObj;

template <typename T>
class CAbstractFactory
{
public:
	static CObj* CreateObj()
	{
		CObj* pObject = new T;

		if (FAILED(pObject->Initialize()))
		{
			SafeDelete(pObject);
			return nullptr;
		}

		return pObject;
	}

	static CObj* CreateObj(const D3DXVECTOR3& vPos)
	{
		CObj* pObject = new T;

		if (FAILED(pObject->Initialize()))
		{
			SafeDelete(pObject);
			return nullptr;
		}

		pObject->SetPos(vPos);

		return pObject;
	}
};

template <typename EFFECT, typename BRIDGE>
class CEffectFactory
{
public:
	static CObj* CreateEffect(const D3DXVECTOR3& vPos, const wstring& wstrStateKey,
		const FRAME& tFrame)
	{
		// 掘⑷類
		BRIDGE* pAnim = new BRIDGE;
		pAnim->Initialize();
		pAnim->SetStateKey(wstrStateKey);
		pAnim->SetFrame(tFrame);

		// 蹺鼻類
		CObj* pEffect = CAbstractFactory<EFFECT>::CreateObj(vPos);
		dynamic_cast<EFFECT*>(pEffect)->SetBridge(pAnim);

		return pEffect;
	}

	static CObj* CreateEffect(const D3DXVECTOR3& vPos, const wstring& wstrStateKey,
		const float& fTime)
	{
		// 掘⑷類
		BRIDGE* pNonAnim = new BRIDGE;
		pNonAnim->Initialize();
		pNonAnim->SetStateKey(wstrStateKey);
		pNonAnim->SetTime(fTime);

		// 蹺鼻類
		CObj* pEffect = CAbstractFactory<EFFECT>::CreateObj(vPos);
		dynamic_cast<EFFECT*>(pEffect)->SetBridge(pNonAnim);

		return pEffect;
	}
};