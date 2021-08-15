// CartoonWar.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CartoonWar.h"
#include "System.h"
#include "MainApp.h"


#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HINSTANCE		g_hInstance;
HWND				g_hWnd;

FbxManager*         g_FbxManager = nullptr;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
_float      g_MaxTime = 60.f;
int			g_iRedNum=0;
int			g_iBlueNum=0;
bool        g_DefferedUIRender = false;
int         g_DefferedRender = 0;
bool        g_InputMode = false;
bool        g_IsCollisionStart = false;
int         g_iPlayerIdx = 0;
int         g_iGold = 0;
int         g_iTotalTime = 0;
bool        g_IsRedWin = false;
bool        g_IsCollisionBox = false;
bool        g_IsNaviMesh = false;


_vec3 g_vEyePt = _vec3(30.f, 100.0f, -20.0f);
_vec3 g_vLookatPt = _vec3(0.0f, 0.f, 0.f);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,                     _In_ int       nCmdShow){
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CARTOONWAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CARTOONWAR));

    MSG msg;

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    CSystem* pSystem = CSystem::GetInstance();
    NULL_CHECK_VAL(pSystem, FALSE);
    pSystem->AddRef();

    CMainApp* pMainApp = CMainApp::Create();
    NULL_CHECK_VAL(pMainApp, FALSE);


    if (FAILED(pSystem->Add_Timer(L"Timer_Default")))
        return FALSE;

    // For.Timer_60
    if (FAILED(pSystem->Add_Timer(L"Timer_60")))
        return FALSE;

    // For.Frame_60
    if (FAILED(pSystem->Add_Frame(L"Frame_60", 100000.f)))
        return FALSE;


    // 기본 메시지 루프입니다:
    while (1)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        _float		fTimeDelta_Default = pSystem->Get_TimeDelta(L"Timer_Default");

        if (true == pSystem->Permit_Call(L"Frame_60", fTimeDelta_Default))
        {
            _float		fTimeDelta_60 = pSystem->Get_TimeDelta(L"Timer_60");
            pMainApp->Update_MainApp(fTimeDelta_60);
            pMainApp->Render_MainApp();
        }
    }



    _ulong		dwRefCnt = 0;
    Safe_Release(pSystem);

    if (dwRefCnt = Safe_Release(pMainApp))
        _MSG_BOX("CMainApp Release Failed");
    g_FbxManager->Destroy();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CARTOONWAR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    RECT rcClient = { 0, 0, WINCX, WINCY };

    AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, TRUE);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    g_hInstance = hInstance;
    g_hWnd = hWnd;

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    //case WM_ACTIVATE:
    //{
    //    CServer_Manager* server = CServer_Manager::GetInstance();
    //    if (nullptr == server)
    //        break;
    //    server->AddRef();
    //    if (LOWORD(wParam) != WA_INACTIVE) // 창 활성화 되어있을때
    //        server->Set_wParam(wParam);
    //    else
    //        server->Set_wParam(wParam);
    //    Safe_Release(server);
    //}
    //break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    //case WM_SOCKET:
    //{
    //    CServer_Manager* server = CServer_Manager::GetInstance();
    //    if (nullptr == server)
    //        break;
    //    server->AddRef();
    //    server->SocketEventMessage(g_hWnd, lParam);
    //    Safe_Release(server);
    //}
    //break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
