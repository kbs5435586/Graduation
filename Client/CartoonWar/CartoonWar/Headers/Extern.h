#pragma once
extern HINSTANCE		g_hInstance;
extern HWND				g_hWnd;


extern D3D12_RASTERIZER_DESC g_arrRSDesc[(UINT)RS_TYPE::END];
extern D3D12_BLEND_DESC g_arrBlendDesc[(UINT)BLEND_TYPE::END];
extern D3D12_DEPTH_STENCIL_DESC g_arrDepthStencilDesc[(UINT)DEPTH_STENCIL_TYPE::END];
extern FbxManager* g_FbxManager;


extern float		g_MaxTime;
	
extern int			g_iRedNum;
extern int			g_iBlueNum;


extern _vec3		g_vEyePt;
extern _vec3		g_vLookatPt;
extern bool			g_DefferedUIRender;
extern int			g_DefferedRender;
extern bool			g_InputMode;
extern bool			g_IsCollisionStart;
extern int			g_iPlayerIdx;