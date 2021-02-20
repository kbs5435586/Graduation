#include "framework.h"
#include "Server_Manager.h"
#include "Management.h"

_IMPLEMENT_SINGLETON(CServer_Manager)
void CServer_Manager::err_quit(const char* msg)
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
CServer_Manager::CServer_Manager()
{

};

void CServer_Manager::MainServer(CManagement* managment)
{
	////////////////////// 엔터 누르고 client_ip가 안비어있을시
	InitServer(g_hWnd);

	cs_packet_login l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_LOGIN;
	int t_id = GetCurrentProcessId();
	//sprintf_s(l_packet.name, "P%03d", t_id % 1000);
	strcpy_s(g_client.name, l_packet.name);
	send_packet(&l_packet);

	//else if (wParam == VK_BACK) // 백스페이스 누르면 ip지워지게
	//{
	//	if (!client_ip.empty())
	//		client_ip.pop_back();
	//}
	//else // 다른키 누를시 ip 입력되게
	//{
	//	client_ip.push_back(wParam);
	//}

	////////////////

	managment = CManagement::GetInstance();
	if (nullptr == managment)
		return;
	managment->AddRef();

	// Layer_Basic 안에 있는 0번째 객체에 접근
	CGameObject* pCube = managment->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_BasicShape", 0);
	// Layer_Basic 안에 있는 0번째 객체의 Transform에 접근
	CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_BasicShape", L"Com_Transform", 0);

	_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

	CGameObject* pTerrain = managment->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", 0);
	CTransform* pTransform_Terrain = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Terrain", L"Com_Transform", 0);


	Safe_Release(managment);
}

void CServer_Manager::Free() // 여기에 소켓, 윈속 종료
{
	if (c_socket)
	{
		shutdown(c_socket, SD_BOTH);
		closesocket(c_socket);
		c_socket = NULL;
	}

	WSACleanup();
}

BOOL CServer_Manager::InitServer(HWND hWnd)
{
	c_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int retval = WSAAsyncSelect(c_socket, hWnd, WM_SOCKET, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (retval == SOCKET_ERROR)
	{
		Free();
		err_quit("WSAAsyncSelect Error\n");
		return FALSE;
	}

	SOCKADDR_IN server_a;
	ZeroMemory(&server_a, sizeof(server_a));
	server_a.sin_family = AF_INET;
	inet_pton(AF_INET, client_IP.c_str(), &server_a.sin_addr);
	server_a.sin_port = htons(SERVER_PORT);

	retval = connect(c_socket, (SOCKADDR*)&server_a, sizeof(server_a));
	if ((retval == SOCKET_ERROR) && (WSAEWOULDBLOCK != WSAGetLastError())) // 비동기 connect는 바로 리턴되면서 WSAEWOULDBLOCK 에러를 발생시킴
	{
		Free();
		err_quit("connect Error\n");
		return FALSE;
	}

	cout << "connect complete!\n";
	return TRUE;
}

void CServer_Manager::ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{
		sc_packet_login_ok* my_packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
		g_myid = my_packet->id;
		avatar.move(my_packet->x, my_packet->y);

		g_left_x = my_packet->x - (SCREEN_WIDTH / 2); // 아바타가 움직일때 카메라가 따라가게 설정해줌
		g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);

		avatar.show();
	}
	break;

	case SC_PACKET_ENTER:
	{
		sc_packet_enter* my_packet = reinterpret_cast<sc_packet_enter*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);

			g_left_x = my_packet->x - (SCREEN_WIDTH / 2); // 아바타가 움직일때 카메라가 따라가게 설정해줌
			g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);

			avatar.show();
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
		sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);

			g_left_x = my_packet->x - (SCREEN_WIDTH / 2); // 아바타가 움직일때 카메라가 따라가게 설정해줌
			g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
		}
		else {
			if (0 != npcs.count(other_id))
				npcs[other_id].move(my_packet->x, my_packet->y);
		}
	}
	break;

	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
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
	case SC_PACKET_CHAT:
	{
		sc_packet_chat* my_packet = reinterpret_cast<sc_packet_chat*>(ptr);
		int o_id = my_packet->id;
		if (0 != npcs.count(o_id))
		{
			npcs[o_id].add_chat(my_packet->message);
		}
	}
	break;
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void CServer_Manager::process_data(char* net_buf, size_t io_byte)
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

void CServer_Manager::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	//size_t sent;
	send(c_socket, p, p[0], 0);
	//g_socket.send(p, p[0], sent);
}

void CServer_Manager::send_move_packet(unsigned char dir)
{
	cs_packet_move m_packet;
	m_packet.type = CS_PACKET_MOVE;
	m_packet.size = sizeof(m_packet);
	m_packet.direction = dir;
	send_packet(&m_packet);
}
