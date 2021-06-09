#include "framework.h"
#include "KeyManager.h"

_IMPLEMENT_SINGLETON(CKeyManager)

CKeyManager::CKeyManager()
{

}
HRESULT CKeyManager::Ready_KeyManager()
{

	return S_OK;
}
void CKeyManager::Key_Update()
{
	m_dwKey = 0;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		m_dwKey |= KEY_LBUTTON;
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		m_dwKey |= KEY_RBUTTON;
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		m_dwKey |= KEY_RETURN;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		m_dwKey |= KEY_SHIFT;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		m_dwKey |= KEY_SPACE;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_dwKey |= KEY_UP;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_dwKey |= KEY_DOWN;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_dwKey |= KEY_LEFT;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_dwKey |= KEY_RIGHT;
	if (GetAsyncKeyState('E') & 0x8000)
		m_dwKey |= KEY_E;
	if (GetAsyncKeyState('Q') & 0x8000)
		m_dwKey |= KEY_Q;

}
_bool CKeyManager::Key_Up(DWORD dwKey)
{
	if (m_dwKey & dwKey)
	{
		m_dwKeyPressed |= dwKey;
		return false;
	}
	else if (m_dwKeyPressed & dwKey)
	{
		m_dwKeyPressed ^= dwKey;
		return true;
	}

	return false;
}
_bool CKeyManager::Key_Down(DWORD dwKey)
{
	if (!(m_dwKeyDown & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyDown |= dwKey;
		return true;
	}
	else if (!(m_dwKey & dwKey) && (m_dwKeyDown & dwKey))
	{
		m_dwKeyDown ^= dwKey;
		return false;
	}

	return false;
}
_bool CKeyManager::Key_Pressing(DWORD dwKey)
{
	if (m_dwKey & dwKey)
		return true;

	return false;
}
_bool CKeyManager::Key_Combine(DWORD dwFirstKey, DWORD dwSecondKey)
{
	if (Key_Pressing(dwSecondKey) && (m_dwKey & dwFirstKey))
		return true;

	return false;
}
CKeyManager* CKeyManager::Create()
{
	CKeyManager* pInstance = new CKeyManager();
	if (FAILED(pInstance->Ready_KeyManager()))
	{
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

void CKeyManager::Free()
{
}
