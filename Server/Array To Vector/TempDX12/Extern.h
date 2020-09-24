#pragma once
extern HINSTANCE g_hInstance;
extern HWND		g_hWnd;
extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice,
    ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE
    d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates =
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3dUploadBuffer =
    NULL);