#pragma once
#include "DXSampleHelper.h"
#include "Win32Application.h"

using Microsoft::WRL::ComPtr;

class D3DAPP
{
public:
	D3DAPP(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

	virtual void OnKeyDown(UINT8 /*key*/) {}
	virtual void OnKeyUp(UINT8 /*key*/) {}

private:
	ComPtr<ID3D12Device> m_device;


public:
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }
	const WCHAR* GetTitle() const { return m_title.c_str(); }
	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false);

private:
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;

	bool m_useWarpDevice;

	std::wstring m_title;

	UINT m_frameIndex;
	UINT m_rtvDescriptorSize;
	
	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};

