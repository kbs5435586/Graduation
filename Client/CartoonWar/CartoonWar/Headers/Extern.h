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
extern bool			g_IsWin;
extern int			g_iPlayerIdx;
extern int			g_iGold;
extern int			g_iTotalTime;
extern float		g_iDiffusePer;
extern bool			g_IsCollisionBox;
extern bool			g_IsNaviMesh;
extern bool			g_IsEnd;
extern bool			g_IsFix;
extern bool			g_IsStageStart;
extern string		g_strIP;