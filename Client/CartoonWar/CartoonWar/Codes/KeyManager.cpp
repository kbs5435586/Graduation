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

	if (GetAsyncKeyState('W') & 0x8000)
		m_dwKey |= KEY_UP;
	if (GetAsyncKeyState('S') & 0x8000)
		m_dwKey |= KEY_DOWN;
	if (GetAsyncKeyState('A') & 0x8000)
		m_dwKey |= KEY_LEFT;
	if (GetAsyncKeyState('D') & 0x8000)
		m_dwKey |= KEY_RIGHT;
	if (GetAsyncKeyState('E') & 0x8000)
		m_dwKey |= KEY_E;
	if (GetAsyncKeyState('Q') & 0x8000)
		m_dwKey |= KEY_Q;

	if (GetAsyncKeyState('1') & 0x8000)
		m_dwKey |= KEY_1;
	if (GetAsyncKeyState('2') & 0x8000)
		m_dwKey |= KEY_2;
	if (GetAsyncKeyState('V') & 0x8000)
		m_dwKey |= KEY_V;

	if (GetAsyncKeyState('3') & 0x8000)
		m_dwKey |= KEY_3;
	if (GetAsyncKeyState('4') & 0x8000)
		m_dwKey |= KEY_4;
	if (GetAsyncKeyState('5') & 0x8000)
		m_dwKey |= KEY_5;

	if (GetAsyncKeyState('I') & 0x8000)
		m_dwKey |= KEY_I;
	if (GetAsyncKeyState('O') & 0x8000)
		m_dwKey |= KEY_O;
	if (GetAsyncKeyState('P') & 0x8000)
		m_dwKey |= KEY_P;
	if (GetAsyncKeyState('Z') & 0x8000)
		m_dwKey |= KEY_Z;
	if (GetAsyncKeyState('X') & 0x8000)
		m_dwKey |= KEY_X;
	if (GetAsyncKeyState('N') & 0x8000)
		m_dwKey |= KEY_N;
	if (GetAsyncKeyState('M') & 0x8000)
		m_dwKey |= KEY_M;
	if (GetAsyncKeyState(VK_F1) & 0x8000)
		m_dwKey |= KEY_F1;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
		m_dwKey |= KEY_F2;
	if (GetAsyncKeyState(VK_F3) & 0x8000)
		m_dwKey |= KEY_F3;
	if (GetAsyncKeyState(VK_F4) & 0x8000)
		m_dwKey |= KEY_F4;
	if (GetAsyncKeyState(VK_F5) & 0x8000)
		m_dwKey |= KEY_F5;
	if (GetAsyncKeyState(VK_F6) & 0x8000)
		m_dwKey |= KEY_F6;
	if (GetAsyncKeyState(VK_F7) & 0x8000)
		m_dwKey |= KEY_F7;
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
