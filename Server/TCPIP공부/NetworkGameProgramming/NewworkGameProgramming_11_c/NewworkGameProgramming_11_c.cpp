// NewworkGameProgramming_11_c.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "framework.h"
#include "NewworkGameProgramming_11_c.h"

using namespace std;

#define MAX_LOADSTRING 100
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    1024

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
SOCKET sock;
string fileName;
static BOOL MYSELECT = FALSE;
static float percent = 0.f;
HANDLE SendEvent;
HANDLE FinishSend;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI ClientMain(LPVOID arg)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	int retval;
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		return 0;

	WaitForSingleObject(hWriteEvent, INFINITE);

	FILE* fp = fopen(fileName.c_str(), "rb");
	int nameSize = fileName.length();

	// 파일 이름 데이터 받기(고정 길이)
	retval = send(sock, (char*)&nameSize, sizeof(nameSize), 0);
	if (retval == SOCKET_ERROR)
		return 0;

	// 파일 이름 데이터 받기(가변 길이)
	retval = send(sock, fileName.c_str(), nameSize, 0);
	if (retval == SOCKET_ERROR)
		return 0;

	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp); // 시작부터 현재 위치까지의 바이트수 읽기

	// 파일 데이터 받기(고정 길이)
	retval = send(sock, (char*)&fileSize, sizeof(fileSize), 0);
	if (retval == SOCKET_ERROR)
		return 0;

	char buf[BUFSIZE] = {};
	int checkSize = 0;
	int fileSend;
	rewind(fp);

	while (1)
	{
		fileSend = fread(buf, 1, BUFSIZE, fp);

		if (fileSend == 0 && checkSize == fileSize)
		{
			checkSize += fileSend;
			break;
		}
		else if (fileSend > 0)
		{
			retval = send(sock, buf, fileSend, 0); // 파일 데이터 받기(가변 길이)
			Sleep(100);
			if (retval == SOCKET_ERROR)
				return 0;
			checkSize += fileSend;
			percent = ((float)checkSize / (float)fileSize) * 100.f;
		}
	}
	fclose(fp);
	closesocket(sock);
	WSACleanup();
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다
	SendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (SendEvent == NULL) return 1;
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_NEWWORKGAMEPROGRAMMING11C, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NEWWORKGAMEPROGRAMMING11C));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NEWWORKGAMEPROGRAMMING11C));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_NEWWORKGAMEPROGRAMMING11C);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 400, 400, nullptr, nullptr, hInstance, nullptr);

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
	switch (message)
	{
	case WM_CHAR:
		if (wParam == VK_BACK)
		{
			if (!fileName.empty())
				fileName.pop_back();
		}
		else
		{
			fileName.push_back(wParam);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_CREATE:
	{
		CreateWindow(TEXT("button"), TEXT("Connect"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			50, 275, 100, 50, hWnd, (HMENU)0, hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("Send File"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			250, 275, 100, 50, hWnd, (HMENU)1, hInst, NULL);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		wstring input_ip = L"Input file name : ";
		wstring inpuy_ip2{ fileName.begin(),fileName.end() };
		wstring textBox = input_ip + inpuy_ip2;

		RECT rt = { 100,50,300,100 };
		DrawText(hdc, textBox.c_str(), -1, &rt, DT_CENTER | DT_NOCLIP);

		HBRUSH nowBrush;
		HBRUSH oldBrush;

		Rectangle(hdc, 50, 200, 350, 250); // 배경

		nowBrush = (HBRUSH)CreateSolidBrush(RGB(128, 64, 0));
		oldBrush = (HBRUSH)SelectObject(hdc, nowBrush);

		Rectangle(hdc, 50, 200, 50 + (percent * 3.f), 250); // 게이지
		InvalidateRect(hWnd, NULL, FALSE);

		SelectObject(hdc, oldBrush);
		DeleteObject(nowBrush);
		EndPaint(hWnd, &ps);
	}
	//InvalidateRect(hWnd, NULL, FALSE);
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 1:
			SetEvent(SendEvent);
			break;
		}
		break;
	case WM_CLOSE:
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