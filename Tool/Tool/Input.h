#pragma once
#include "Base.h"
class CInput :
	public CBase
{
	_DECLARE_SINGLETON(CInput)
public:
	CInput();
	virtual ~CInput()=default;
public:
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void SetUp_InputState();
public:
	_byte Get_DIKeyState(_ubyte byKeyID) {return m_KeyState[byKeyID];}
	_byte Get_DIMouseState(_ubyte byKeyID) {return m_MouseState.rgbButtons[byKeyID];}
	LONG Get_DIMouseMove(_ubyte byKeyID) {return *((long*)&m_MouseState + byKeyID);}
private:
	LPDIRECTINPUT8			m_pSDK = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;
private:
	_byte					m_KeyState[256];
	DIMOUSESTATE			m_MouseState;
private:
	HRESULT Ready_KeyBoard(HWND hWnd);
	HRESULT Ready_Mouse(HWND hWnd);
public:
	virtual void Free();
};

