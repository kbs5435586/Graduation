// GameServer_Chess.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#include <iostream>
#include "stdafx.h"
#include "GameServer_Chess.h"
#include "protocol.h"

using namespace std;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

Client_Info Player_Data;
SOCKET s_socket; // 클라이언트와 연결할 소켓
string client_ip;
constexpr int BUF_SIZE = 1024;
constexpr short PORT = 3500;
int g_myid;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 메인 함수
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, // H는 핸들, 
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMESERVERCHESS, szWindowClass, MAX_LOADSTRING); // 창 제목
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMESERVERCHESS));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    MSG msg;

    //WPARAM      wParam; 키보드 관련 정보
    //LPARAM      lParam; 마우스 관련 정보

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0)) // while(1) 과 같은 기능
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

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

    wcex.style          = CS_HREDRAW | CS_VREDRAW; // 가로 세로 변화 감지
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMESERVERCHESS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCEW(IDC_GAMESERVERCHESS); // 상단 메뉴
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,  // || 이용하여 사용할거 추가
      CW_USEDEFAULT, 0, 950, 839, nullptr, nullptr, hInstance, nullptr); // 실제 윈도우 창 만드는 부분

   /*
   WS_OVERLAPPEDWINDOW = 윈도우창 각종 설정들

   앞 CW_USEDEFAULT, 0 = 윈도우 창 생성위치
   뒤 CW_USEDEFAULT, 0 = 윈도우 창 크기
   
   */

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int PieceX = 0, PieceY = 0;
    switch (message)
    {
    case WM_CHAR:
        if (wParam == VK_RETURN && !client_ip.empty())
        {
            SOCKADDR_IN server_a;
            ZeroMemory(&server_a, sizeof(server_a));
            server_a.sin_family = AF_INET;
            inet_pton(AF_INET, client_ip.c_str(), &server_a.sin_addr);
            server_a.sin_port = htons(PORT);

            connect(s_socket, (SOCKADDR*)&server_a, sizeof(server_a));

            CtoS_packet_login l_packet;
            l_packet.size = sizeof(l_packet);
            l_packet.type = CS_LOGIN;
            int t_id = GetCurrentProcessId();
            sprintf_s(l_packet.name, "P%03d", t_id % 1000);
            strcpy_s(Player_Data.name, l_packet.name);
            send_packet(&l_packet);
        }
        else if (wParam == VK_BACK)
        {
            if (!client_ip.empty())
                client_ip.pop_back();
        }
        else
        {
            client_ip.push_back(wParam);
        }
        break;
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
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:
            Player_Data.key = 'w';
            send_move_packet(MV_UP);
            break;
        case VK_DOWN:
            Player_Data.key = 's';
            send_move_packet(MV_DOWN);
            break;
        case VK_LEFT:
            Player_Data.key = 'a';
            send_move_packet(MV_LEFT);
            break;
        case VK_RIGHT:
            Player_Data.key = 'd';
            send_move_packet(MV_RIGHT);
            break;
        case VK_ESCAPE:
            closesocket(s_socket);
            WSACleanup();
            PostQuitMessage(0);
            break;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        wstring input_ip = L"Server IP : ";
        wstring inpuy_ip2{ client_ip.begin(),client_ip.end() };
        wstring textBox = input_ip + inpuy_ip2;

        PieceX = Player_Data.m_position.x * 100;
        PieceY = Player_Data.m_position.y * 100;

        HBRUSH nowBrush;
        HBRUSH oldBrush;

        nowBrush = (HBRUSH)CreateSolidBrush(RGB(128, 64, 0));
        oldBrush = (HBRUSH)SelectObject(hdc, nowBrush);

        Rectangle(hdc, 0, 0, 800, 800);

        nowBrush = (HBRUSH)CreateSolidBrush(RGB(172, 160, 79));
        oldBrush = (HBRUSH)SelectObject(hdc, nowBrush);

        for (int i = 0; i < 4; ++i)
        {
            Rectangle(hdc, 0, i * 200, 100, (i * 200) + 100);
            Rectangle(hdc, 200, i * 200, 300, (i * 200) + 100);
            Rectangle(hdc, 400, i * 200, 500, (i * 200) + 100);
            Rectangle(hdc, 600, i * 200, 700, (i * 200) + 100);

            Rectangle(hdc, 100, (i * 200) + 100, 200, (i * 200) + 200);
            Rectangle(hdc, 300, (i * 200) + 100, 400, (i * 200) + 200);
            Rectangle(hdc, 500, (i * 200) + 100, 600, (i * 200) + 200);
            Rectangle(hdc, 700, (i * 200) + 100, 800, (i * 200) + 200);
        }

        nowBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
        oldBrush = (HBRUSH)SelectObject(hdc, nowBrush);

        Ellipse(hdc, 0 + PieceX, 0 + PieceY, 100 + PieceX, 100 + PieceY);
        RECT rt = { 100,100,400,300 };
        DrawText(hdc, textBox.c_str(), -1, &rt, DT_CENTER | DT_WORDBREAK);

        SelectObject(hdc, oldBrush);
        DeleteObject(nowBrush);
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

void ProcessPacket(char* ptr)
{
    static bool first_time = true;
    switch (ptr[1])
    {
    case SC_PACKET_LOGIN_OK:
    {
        StoC_packet_login_ok* my_packet = reinterpret_cast<StoC_packet_login_ok*>(ptr);
        g_myid = my_packet->id;
        Player_Data.m_position.x = my_packet->x;
        Player_Data.m_position.y = my_packet->y;
    }
    break;

    case SC_PACKET_ENTER:
    {
        StoC_packet_enter* my_packet = reinterpret_cast<StoC_packet_enter*>(ptr);
        int id = my_packet->id;

        if (id == g_myid) {
            g_myid = my_packet->id;
            Player_Data.m_position.x = my_packet->x;
            Player_Data.m_position.y = my_packet->y;
        }
        else {
            if (id < NPC_ID_START)
                npcs[id] = OBJECT{ *pieces, 64, 0, 64, 64 };
            else
                npcs[id] = OBJECT{ *pieces, 0, 0, 64, 64 };
            strcpy_s(npcs[id].name, my_packet->name);
            npcs[id].set_name(my_packet->name);
            npcs[id].move(my_packet->x, my_packet->y);
            npcs[id].show();
        }
    }
    break;
    case SC_PACKET_MOVE:
    {
        StoC_packet_move* my_packet = reinterpret_cast<StoC_packet_move*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) {
            g_myid = my_packet->id;
            Player_Data.m_position.x = my_packet->x;
            Player_Data.m_position.y = my_packet->y;
        }
        else {
            if (0 != npcs.count(other_id))
                npcs[other_id].move(my_packet->x, my_packet->y);
        }
    }
    break;

    case SC_PACKET_LEAVE:
    {
        StoC_packet_leave* my_packet = reinterpret_cast<StoC_packet_leave*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) {
            avatar.hide();
        }
        else {
            if (0 != npcs.count(other_id))
                npcs[other_id].hide();
        }
    }
    break;
    default:
        printf("Unknown PACKET type [%d]\n", ptr[1]);

    }
}

void process_data(char* net_buf, size_t io_byte)
{
    char* ptr = net_buf;
    static size_t in_packet_size = 0;
    static size_t saved_packet_size = 0;
    static char packet_buffer[BUF_SIZE];

    while (0 != io_byte) {
        if (0 == in_packet_size) in_packet_size = ptr[0];
        if (io_byte + saved_packet_size >= in_packet_size) {
            memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
            ProcessPacket(packet_buffer);
            ptr += in_packet_size - saved_packet_size;
            io_byte -= in_packet_size - saved_packet_size;
            in_packet_size = 0;
            saved_packet_size = 0;
        }
        else {
            memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
            saved_packet_size += io_byte;
            io_byte = 0;
        }
    }
}

void send_packet(void* packet)
{
    char* p = reinterpret_cast<char*>(packet);
    size_t sent;
    send(s_socket, p, p[0], sent);
    int a = 3;
}

void send_move_packet(unsigned char dir)
{
    CtoS_packet_move m_packet;
    m_packet.type = CS_MOVE;
    m_packet.size = sizeof(m_packet);
    m_packet.direction = dir;
    send_packet(&m_packet);
}