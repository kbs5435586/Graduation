#pragma once
extern HINSTANCE		g_hInstance;
extern HWND				g_hWnd;

extern D3D12_RASTERIZER_DESC g_arrRSDesc[(UINT)RS_TYPE::END];
extern D3D12_BLEND_DESC g_arrBlendDesc[(UINT)BLEND_TYPE::END];

extern FbxManager* g_FbxManager;
