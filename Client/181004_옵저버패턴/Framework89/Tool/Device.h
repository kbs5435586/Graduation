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
	// LPDIRECT3D9: Directx���� ���� ��ġ�� �����ϱ� ���� Com��ü.
	LPDIRECT3D9			m_p3D;

	// LPDIRECT3DDEVICE9: Directx���� �׷��� ��ġ�� �����ϱ� ���� Com��ü.
	LPDIRECT3DDEVICE9	m_pDevice;

	// LPD3DXSPRITE: Directx���� 2D �̹����� �������ϱ� ���� Com��ü.
	LPD3DXSPRITE		m_pSprite;

	// LPD3DXFONT: Directx���� ��Ʈ�� �������ϱ� ���� Com��ü.
	LPD3DXFONT			m_pFont;

	// COM (Component Object Model)
};

