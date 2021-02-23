#pragma once
class CDevice
{
	DECLARE_SINGLETON(CDevice)

private:
	CDevice();
	~CDevice();

public:
	LPDIRECT3DDEVICE9 GetDevice() { return m_pDevice; }
	LPD3DXSPRITE GetSprite() { return m_pSprite; }
	LPD3DXFONT GetFont() { return m_pFont; }

public:
	HRESULT InitDevice();
	void Render_Begin();
	void Render_End(HWND hWnd = nullptr);
	void Release();

private:
	void SetParameters(D3DPRESENT_PARAMETERS& d3dpp);

private:
	// LPDIRECT3D9: Directx에서 여러 장치를 제어하기 위한 Com객체.
	LPDIRECT3D9			m_p3D;

	// LPDIRECT3DDEVICE9: Directx에서 그래픽 장치를 제어하기 위한 Com객체.
	LPDIRECT3DDEVICE9	m_pDevice;

	// LPD3DXSPRITE: Directx에서 2D 이미지를 렌더링하기 위한 Com객체.
	LPD3DXSPRITE		m_pSprite;

	// LPD3DXFONT: Directx에서 폰트를 렌더링하기 위한 Com객체.
	LPD3DXFONT			m_pFont;

	// COM (Component Object Model)
};

