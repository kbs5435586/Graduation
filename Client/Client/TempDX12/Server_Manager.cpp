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

	

	// Layer_Cube 안에 있는 0번째 객체에 접근
	CGameObject* pCube = managment->Get_GameObject((_uint)SCENEID::SCENE_LOGO, L"Layer_Cube", 0);

	CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
		L"Layer_Cube", L"Com_Transform", 0);
	_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

	CGameObject* pTerrain = managment->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", 0);
	CTransform* pTransform_Terrain = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		L"Layer_Terrain", L"Com_Transform", 0);


	Safe_Release(managment);
}

BOOL CServer_Manager::InitServer(HWND hWnd)
{
	m_cSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int retval = WSAAsyncSelect(m_cSocket, hWnd, WM_SOCKET, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (retval == SOCKET_ERROR)
	{
		Free();
		err_quit("WSAAsyncSelect Error\n");
		return FALSE;
	}

	SOCKADDR_IN server_a;
	ZeroMemory(&server_a, sizeof(server_a));
	server_a.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &server_a.sin_addr);
	server_a.sin_port = htons(SERVER_PORT);

	retval = connect(m_cSocket, (SOCKADDR*)&server_a, sizeof(server_a));
	if ((retval == SOCKET_ERROR) && (WSAEWOULDBLOCK != WSAGetLastError())) // 비동기 connect는 바로 리턴되면서 WSAEWOULDBLOCK 에러를 발생시킴
	{
		Free();
		err_quit("connect Error\n");
		return FALSE;
	}

	//cout << "connect complete!\n";
	return TRUE;
}

void CServer_Manager::ProcessPacket(char* ptr)
{
	static bool first_time = true;

	switch (ptr[1])
	{
	case SC_PACKET_LOGIN_OK:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();

		sc_packet_login_ok* my_packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
		m_myid = my_packet->id;

		CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
			L"Layer_Cube", L"Com_Transform", m_myid);
		_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

		vPos.x = m_player.x = my_packet->x;
		vPos.y = m_player.y = my_packet->y;
		vPos.z = m_player.z = my_packet->z;
		pTransform_Cube->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		cool_time = high_resolution_clock::now(); // 임시 NPC 소환 쿨타임 초기화
		m_player.showCharacter = true;

		Safe_Release(managment);
	}
	break;

	case SC_PACKET_ENTER:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();

		sc_packet_enter* my_packet = reinterpret_cast<sc_packet_enter*>(ptr);
		int recv_id = my_packet->id;

		if (recv_id == m_myid) 
		{
			CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Cube", L"Com_Transform", recv_id);
			_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

			vPos.x = m_player.x = my_packet->x;
			vPos.y = m_player.y = my_packet->y;
			vPos.z = m_player.z = my_packet->z;
			m_player.showCharacter = true;
			pTransform_Cube->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		}
		else
		{
			//if (recv_id < NPC_ID_START) // 다른 플레이어 일때
			//{
			//	CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
			//		L"Layer_Cube", L"Com_Transform", recv_id);
			//	_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);
			//	strcpy_s(m_npcs[recv_id].name, my_packet->name);
			//	vPos.x = m_npcs[recv_id].x = my_packet->x;
			//	vPos.y = m_npcs[recv_id].y = my_packet->y;
			//	vPos.z = m_npcs[recv_id].z = my_packet->z;
			//	m_npcs[recv_id].showCharacter = true;
			//	pTransform_Cube->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
			//}
			//else // NPC 일때
			//{
			CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Rect", L"Com_Transform", recv_id - 30);
			_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);
			strcpy_s(m_npcs[recv_id].name, my_packet->name);
			vPos.x = m_npcs[recv_id].x = my_packet->x;
			vPos.y = m_npcs[recv_id].y = my_packet->y;
			vPos.z = m_npcs[recv_id].z = my_packet->z;
			m_npcs[recv_id - 30].showCharacter = true;
			pTransform_Cube->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
			/*	}*/
		}
		Safe_Release(managment);
	}
	break;
	case SC_PACKET_MOVE:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();

		sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
		int recv_id = my_packet->id;

		if (recv_id == m_myid) 
		{
			CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Cube", L"Com_Transform", recv_id);

			_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

			vPos.x = my_packet->x;
			vPos.y = my_packet->y;
			vPos.z = my_packet->z;
			pTransform_Cube->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		}
		else // NPC 
		{ 
			if (0 != m_npcs.count(recv_id))
			{
				CTransform* pTransform_Cube = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
					L"Layer_Rect", L"Com_Transform", recv_id - 30);

				_vec3 vPos = *pTransform_Cube->Get_StateInfo(CTransform::STATE_POSITION);

				vPos.x = my_packet->x;
				vPos.y = my_packet->y;
				vPos.z = my_packet->z;
				pTransform_Cube->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
			}
		}
		Safe_Release(managment);
	}
	break;
	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
		int other_id = my_packet->id;
		if (other_id == m_myid) {
			m_player.showCharacter = false;
		}
		else {
			if (0 != m_npcs.count(other_id))
				m_npcs[other_id].showCharacter = false;
		}
	}
	break;
	//case SC_PACKET_CHAT:
	//{
	//	sc_packet_chat* my_packet = reinterpret_cast<sc_packet_chat*>(ptr);
	//	int o_id = my_packet->id;
	//	if (0 != m_npcs.count(o_id))
	//	{
	//		m_npcs[o_id].add_chat(my_packet->message);
	//	}
	//}
	//break;
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

void CServer_Manager::SocketEventMessage(HWND hWnd, LPARAM lParam)
{
	static int count = 0;
	char TempLog[256];

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
	{
		m_player.showCharacter = false;
		send_login_ok_packet();
	}
	break;
	case FD_READ:
	{
		char net_buf[MAX_BUF_SIZE];
		auto recv_result = recv(m_cSocket, net_buf, MAX_BUF_SIZE, 0);
		if (recv_result == SOCKET_ERROR)
		{
			//Free();
			//wcout << L"Recv 에러!";pc
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
		Free();
		sprintf(TempLog, "gSock FD_CLOSE");
		break;

	default:
		sprintf(TempLog, "gSock Unknown event received: %d", WSAGETSELECTEVENT(lParam));
		break;
	}
}

void CServer_Manager::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	//size_t sent;
	send(m_cSocket, p, p[0], 0);
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

void CServer_Manager::send_login_ok_packet()
{
	cs_packet_login l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_LOGIN;
	int t_id = GetCurrentProcessId();
	sprintf_s(l_packet.name, "P%03d", t_id % 1000);
	strcpy_s(m_player.name, l_packet.name);
	send_packet(&l_packet);
}

void CServer_Manager::send_add_npc_packet()
{
	if (m_myid < NPC_ID_START)
	{
		cs_packet_add_npc l_packet;
		l_packet.size = sizeof(l_packet);
		l_packet.type = CS_PACKET_ADD_NPC;
		l_packet.id = m_myid;
		send_packet(&l_packet);
	}
}

void CServer_Manager::send_npc_act_packet(unsigned char act)
{
	if (m_myid < NPC_ID_START)
	{
		cs_packet_npc_act l_packet;
		l_packet.size = sizeof(l_packet);
		l_packet.type = CS_PACKET_NPC_ACT;
		l_packet.id = m_myid;
		l_packet.act = act;
		send_packet(&l_packet);
	}
}

void CServer_Manager::disconnect()
{
	if (m_cSocket)
	{
		shutdown(m_cSocket, SD_BOTH);
		closesocket(m_cSocket);
		m_cSocket = NULL;
	}
	WSACleanup();
}

void CServer_Manager::Free() // 여기에 소켓, 윈속 종료
{
	disconnect();
}

bool CServer_Manager::Get_ShowPlayer()
{
	return m_player.showCharacter;
}

bool CServer_Manager::Get_ShowNPC()
{
	return m_npcs[m_myid].showCharacter;
}

char CServer_Manager::Get_Lastorder()
{
	return last_order;
}

void CServer_Manager::Set_LastOrder(char order)
{
	last_order = order;
}

high_resolution_clock::time_point CServer_Manager::Get_Cooltime()
{
	return cool_time;
}

void CServer_Manager::Set_CoolTime(high_resolution_clock::time_point ct)
{
	cool_time = ct;
}
