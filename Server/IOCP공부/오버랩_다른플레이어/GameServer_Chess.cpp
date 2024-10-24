﻿#include "stdafx.h"
#include "GameServer_Chess.h"
#include "protocol.h"

using namespace std;

#define WM_SOCKET (WM_USER+1)
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

ClientInfo player;
unordered_map<int, ClientInfo> npcs;

SOCKET c_socket; // 클라이언트와 연결할 소켓
string client_ip;
int g_myid;
int NPC_ID_START = 10000;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);

    LocalFree(lpMsgBuf);
    exit(1);
}
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);

    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, // H는 핸들, 
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

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
    static int PlayerX = 0, PlayerY = 0;
    //static int OtherX = 0, OtherY = 0;
    switch (message)
    {
    case WM_CHAR:
        if (wParam == VK_RETURN && !client_ip.empty())
        {
            InitServer(hWnd);
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
    case WM_SOCKET:
    {
        SocketEventMessage(hWnd, lParam);
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
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:
            send_move_packet(D_UP);
            break;
        case VK_DOWN:
            send_move_packet(D_DOWN);
            break;
        case VK_LEFT:
            send_move_packet(D_LEFT);
            break;
        case VK_RIGHT:
            send_move_packet(D_RIGHT);
            break;
        case VK_ESCAPE:
            EndSocketConnect(c_socket);
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

        PlayerX = player.m_x * 100;
        PlayerY = player.m_y * 100;

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

        Ellipse(hdc, 0 + PlayerX, 0 + PlayerY, 100 + PlayerX, 100 + PlayerY);
        RECT rt = { 675,0,875,300 };
        DrawText(hdc, textBox.c_str(), -1, &rt, DT_RIGHT | DT_WORDBREAK);

        nowBrush = (HBRUSH)CreateSolidBrush(RGB(125, 0, 0));
        oldBrush = (HBRUSH)SelectObject(hdc, nowBrush);

        for (auto& others : npcs)
        {
            if (true == others.second.showCharacter)
            {
                int OtherX = others.second.m_x * 100, OtherY = others.second.m_y * 100;
                Ellipse(hdc, 0 + OtherX, 0 + OtherY, 100 + OtherX, 100 + OtherY);
            }
        }

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

BOOL InitServer(HWND hWnd)
{
    c_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    int retval = WSAAsyncSelect(c_socket, hWnd, WM_SOCKET, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
    if (retval == SOCKET_ERROR)
    {
        EndSocketConnect(c_socket);
        err_quit("WSAAsyncSelect Error\n");
        return FALSE;
    }

    SOCKADDR_IN server_a;
    ZeroMemory(&server_a, sizeof(server_a));
    server_a.sin_family = AF_INET;
    inet_pton(AF_INET, client_ip.c_str(), &server_a.sin_addr);
    server_a.sin_port = htons(SERVER_PORT);

    retval = connect(c_socket, (SOCKADDR*)&server_a, sizeof(server_a));
    if ((retval == SOCKET_ERROR) && (WSAEWOULDBLOCK != WSAGetLastError())) // 비동기 connect는 바로 리턴되면서 WSAEWOULDBLOCK 에러를 발생시킴
    {
        EndSocketConnect(c_socket);
        err_quit("connect Error\n");
        return FALSE;
    }

    cout << "connect complete!\n";
    return TRUE;
}

void ProcessPacket(char* ptr)
{
    static bool first_time = true;
    switch (ptr[1])
    {
    case SC_PACKET_LOGIN_OK:
    {
        sc_packet_login_ok* my_packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
        g_myid = my_packet->id;
        player.m_x = my_packet->x;
        player.m_y = my_packet->y;
        player.showCharacter = true;
    }
    break;

    case SC_PACKET_ENTER:
    {
        sc_packet_enter* my_packet = reinterpret_cast<sc_packet_enter*>(ptr);
        int id = my_packet->id;

        if (id == g_myid) {
            player.m_x = my_packet->x;
            player.m_y = my_packet->y;
            player.showCharacter = true;
        }
        else {
            //if (id < NPC_ID_START)
            //    npcs[id] = OBJECT{ *pieces, 64, 0, 64, 64 };
            //else
            //    npcs[id] = OBJECT{ *pieces, 0, 0, 64, 64 };
            strcpy_s(npcs[id].m_name, my_packet->name);
            npcs[id].m_x = my_packet->x;
            npcs[id].m_y = my_packet->y;
            npcs[id].showCharacter = true;
        }
    }
    break;
    case SC_PACKET_MOVE:
    {
        sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) {
            player.m_x = my_packet->x;
            player.m_y = my_packet->y;
        }
        else {
            if (0 != npcs.count(other_id))
            {
                npcs[other_id].m_x = my_packet->x;
                npcs[other_id].m_y = my_packet->y;
            }
        }
    }
    break;

    case SC_PACKET_LEAVE:
    {
        sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
        int other_id = my_packet->id;
        if (other_id == g_myid) {
            player.showCharacter = false;
        }
        else {
            if (0 != npcs.count(other_id))
                npcs[other_id].showCharacter = false;
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
    static char packet_buffer[MAX_BUF_SIZE];

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

void SocketEventMessage(HWND hWnd, LPARAM lParam)
{
    static int count = 0;
    char TempLog[256];

    switch (WSAGETSELECTEVENT(lParam))
    {
    case FD_CONNECT:
    {
        player.m_x = 4;
        player.m_y = 4;
        player.showCharacter = false;

        cs_packet_login l_packet;
        l_packet.size = sizeof(l_packet);
        l_packet.type = CS_PACKET_LOGIN;
        int t_id = GetCurrentProcessId();
        sprintf_s(l_packet.name, "P%03d", t_id % 1000);
        strcpy(player.m_name, l_packet.name);
        //avatar.set_name(l_packet.name);
        send_packet(&l_packet);
    }
    break;
    case FD_READ:
    {
        char net_buf[MAX_BUF_SIZE];
        auto recv_result = recv(c_socket, net_buf, MAX_BUF_SIZE, 0);
        if (recv_result == SOCKET_ERROR)
        {
            wcout << L"Recv 에러!";
            while (true);
        }
        else if (recv_result == 0)
            break;
        
        if (recv_result > 0)
            process_data(net_buf, recv_result);
    }
       // ProcessPacket(char* ptr);
        sprintf(TempLog, "gSock FD_READ");
        break;
    case FD_WRITE:


        sprintf(TempLog, "gSock FD_WRITE");
        break;

    case FD_CLOSE:
        EndSocketConnect(c_socket);
        sprintf(TempLog, "gSock FD_CLOSE");
        break;

    default:
        sprintf(TempLog, "gSock Unknown event received: %d", WSAGETSELECTEVENT(lParam));
        break;
    }
}

void send_packet(void* packet)
{
    char* p = reinterpret_cast<char*>(packet);
    //size_t sent;
    send(c_socket, p, p[0], 0);
    //g_socket.send(p, p[0], sent);
}

void send_move_packet(unsigned char dir)
{
    cs_packet_move m_packet;
    m_packet.type = CS_PACKET_MOVE;
    m_packet.size = sizeof(m_packet);
    m_packet.direction = dir;
    send_packet(&m_packet);
}

void EndSocketConnect(SOCKET& socket)
{
    if (socket)
    {
        shutdown(socket, SD_BOTH);
        closesocket(socket);
        socket = NULL;
    }
}
