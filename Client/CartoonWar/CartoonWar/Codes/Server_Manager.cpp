#include "framework.h"
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
	////////////////////// ���� ������ client_ip�� �Ⱥ��������

	//else if (wParam == VK_BACK) // �齺���̽� ������ ip��������
	//{
	//	if (!client_ip.empty())
	//		client_ip.pop_back();
	//}
	//else // �ٸ�Ű ������ ip �Էµǰ�
	//{
	//	client_ip.push_back(wParam);
	//}

	////////////////

	

	// Layer_Cube �ȿ� �ִ� 0��° ��ü�� ����
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
	if ((retval == SOCKET_ERROR) && (WSAEWOULDBLOCK != WSAGetLastError())) // �񵿱� connect�� �ٷ� ���ϵǸ鼭 WSAEWOULDBLOCK ������ �߻���Ŵ
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
		m_player.id = my_packet->id;
		CTransform* pTransform;
		if (0 == m_player.id)
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Cube", L"Com_Transform", 0);
		}
		else if (1 == m_player.id)
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Wire", L"Com_Transform", 0);
		}
		_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);

		vPos.x = m_player.x = my_packet->x;
		vPos.y = m_player.y = my_packet->y;
		vPos.z = m_player.z = my_packet->z;
		pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		add_npc_ct = high_resolution_clock::now(); // �ӽ� NPC ��ȯ ��Ÿ�� �ʱ�ȭ
		change_formation_ct = high_resolution_clock::now(); // �ӽ� NPC ��ȯ ��Ÿ�� �ʱ�ȭ
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
		CTransform* pTransform;

		if (recv_id == m_player.id)
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Cube", L"Com_Transform", recv_id);
			_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);

			vPos.x = m_player.x = my_packet->x;
			vPos.y = m_player.y = my_packet->y;
			vPos.z = m_player.z = my_packet->z;
			m_player.showCharacter = true;
			pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		}
		else
		{
			if (recv_id < NPC_ID_START) // �ٸ� �÷��̾� �϶�
			{
				if (0 == recv_id) // �ٸ� �÷��̾� �϶�
				{
					pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
						L"Layer_Cube", L"Com_Transform", 0);
				}
				if (1 == recv_id) // �ٸ� �÷��̾� �϶�
				{
					pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
						L"Layer_Wire", L"Com_Transform", 0);
				}
			}
			else // NPC �϶�
			{
				pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
					L"Layer_Rect", L"Com_Transform", npc_id_to_idx(recv_id));
			}
			_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
			strcpy_s(m_npcs[recv_id].name, my_packet->name);
			vPos.x = m_npcs[recv_id].x = my_packet->x;
			vPos.y = m_npcs[recv_id].y = my_packet->y;
			vPos.z = m_npcs[recv_id].z = my_packet->z;
			m_npcs[recv_id].showCharacter = true;
			pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
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
		CTransform* pTransform;
		if (recv_id == m_player.id)
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Cube", L"Com_Transform", 0);
		}
		else if (recv_id < NPC_ID_START) // �ٸ� �÷��̾�
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Wire", L"Com_Transform", 0);
		}
		else // NPC 
		{ 
			if (0 != m_npcs.count(recv_id))
			{
				pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
					L"Layer_Rect", L"Com_Transform", npc_id_to_idx(recv_id));
			}
		}
		_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
		vPos.x = my_packet->x;
		vPos.y = my_packet->y;
		vPos.z = my_packet->z;
		pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		Safe_Release(managment);
	}
	break;
	case SC_PACKET_ROTATE:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();

		sc_packet_rotate* my_packet = reinterpret_cast<sc_packet_rotate*>(ptr);
		int recv_id = my_packet->id;
		CTransform* pTransform;

		if (recv_id == m_player.id)
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
				L"Layer_Cube", L"Com_Transform", 0);
		}
		else // NPC 
		{
			if (recv_id < NPC_ID_START) // �ٸ� �÷��̾�
			{
				pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
					L"Layer_Wire", L"Com_Transform", 0);
			}
			else
			{
				if (0 != m_npcs.count(recv_id))
				{
					pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_LOGO,
						L"Layer_Rect", L"Com_Transform", npc_id_to_idx(recv_id));
				}
			}
		}
		_vec3 rPos = *pTransform->Get_StateInfo(CTransform::STATE_RIGHT);
		_vec3 uPos = *pTransform->Get_StateInfo(CTransform::STATE_UP);
		_vec3 lPos = *pTransform->Get_StateInfo(CTransform::STATE_LOOK);

		rPos.x = my_packet->r_x, rPos.y = my_packet->r_y, rPos.z = my_packet->r_z;
		uPos.x = my_packet->u_x, uPos.y = my_packet->u_y, uPos.z = my_packet->u_z;
		lPos.x = my_packet->l_x, lPos.y = my_packet->l_y, lPos.z = my_packet->l_z;

		pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &rPos);
		pTransform->Set_StateInfo(CTransform::STATE_UP, &uPos);
		pTransform->Set_StateInfo(CTransform::STATE_LOOK, &lPos);
		Safe_Release(managment);
	}
	break;
	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
		int other_id = my_packet->id;
		if (other_id == m_player.id)
		{
			m_player.showCharacter = false;
		}
		else 
		{
			if (0 != m_npcs.count(other_id))
				m_npcs[other_id].showCharacter = false;
		}
	}
	break;
	case SC_PACKET_ADD_NPC_OK:
	{
		sc_packet_npc_add_ok* my_packet = reinterpret_cast<sc_packet_npc_add_ok*>(ptr);
		int npc_id = my_packet->id;
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
			//wcout << L"Recv ����!";pc
			while (true);
		}
		else if (recv_result == 0)
			break;

		if (recv_result > 0)
			process_data(net_buf, recv_result);
	}
	// ProcessPacket(char* ptr);
	break;
	case FD_WRITE:
		break;
	case FD_CLOSE:
		Free();
		break;
	default:
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

void CServer_Manager::send_rotate_packet(unsigned char dir)
{
	cs_packet_rotate m_packet;
	m_packet.type = CS_PACKET_ROTATE;
	m_packet.size = sizeof(m_packet);
	m_packet.dir = dir;
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
	cs_packet_add_npc l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_ADD_NPC;
	send_packet(&l_packet);
}

void CServer_Manager::send_change_formation_packet()
{
	cs_packet_change_formation l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_CHANGE_FORMATION;
	send_packet(&l_packet);
}

void CServer_Manager::update_key_input()
{
	if ((GetAsyncKeyState('1') & 0x8000))
	{
		send_npc_act_packet(DO_ATTACK);
	}
	if ((GetAsyncKeyState('2') & 0x8000))
	{
		send_npc_act_packet(DO_DEFENCE);
	}
	if ((GetAsyncKeyState('3') & 0x8000))
	{
		send_npc_act_packet(DO_HOLD);
	}
	if ((GetAsyncKeyState('4') & 0x8000))
	{
		send_npc_act_packet(DO_FOLLOW);
	}
	if ((GetAsyncKeyState('5') & 0x8000))
	{
		send_npc_act_packet(DO_RANDMOVE);
	}
	if ((GetAsyncKeyState('6') & 0x8000))
	{
		duration<double> cool_time = duration_cast<duration<double>>(high_resolution_clock::now()
			- Get_ChangeFormation_Cooltime());
		if (cool_time.count() > 2) // �� ��Ÿ�� 2�� ������ִ� ��
		{
			send_change_formation_packet();
			Set_ChangeFormation_CoolTime(high_resolution_clock::now());
		}
	}

	if (GetAsyncKeyState('M') & 0x8000)
	{
		duration<double> cool_time = duration_cast<duration<double>>(high_resolution_clock::now()
			- Get_AddNPC_Cooltime());
		if (cool_time.count() > 2) // �� ��Ÿ�� 2�� ������ִ� ��
		{
			send_add_npc_packet();
			Set_AddNPC_CoolTime(high_resolution_clock::now());
		}
	}
	if (GetAsyncKeyState('T') & 0x8000)
	{
		send_move_packet(GO_FORWARD);
	}
	if (GetAsyncKeyState('F') & 0x8000)
	{
		send_move_packet(GO_LEFT);
	}
	if (GetAsyncKeyState('G') & 0x8000)
	{
		send_move_packet(GO_BACK);
	}
	if (GetAsyncKeyState('H') & 0x8000)
	{
		send_move_packet(GO_RIGHT);
	}
	if (GetAsyncKeyState('O') & 0x8000)
	{
		send_rotate_packet(TURN_LEFT);
	}
	if (GetAsyncKeyState('P') & 0x8000)
	{
		send_rotate_packet(TURN_RIGHT);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		disconnect();
		PostQuitMessage(0);
	}
}

short CServer_Manager::player_index(unsigned short id)
{
	return id - 30;
}

short CServer_Manager::npc_idx_to_id(unsigned short id)
{
	return id + 29;
}

short CServer_Manager::npc_id_to_idx(unsigned short id)
{
	return id - 29;;
}

void CServer_Manager::send_npc_act_packet(unsigned char act)
{
	if (m_player.id < NPC_ID_START)
	{
		cs_packet_npc_act l_packet;
		l_packet.size = sizeof(l_packet);
		l_packet.type = CS_PACKET_NPC_ACT;
		l_packet.id = m_player.id;
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

void CServer_Manager::Free() // ���⿡ ����, ���� ����
{
	disconnect();
}

bool CServer_Manager::Get_ShowPlayer()
{
	return m_player.showCharacter;
}

bool CServer_Manager::Get_ShowNPC(int npc_index)
{
	return m_npcs[npc_idx_to_id(npc_index)].showCharacter;
}

short CServer_Manager::Get_PlayerID()
{
	return m_player.id;
}

WPARAM CServer_Manager::Get_wParam()
{
	return m_wparam;
}

high_resolution_clock::time_point CServer_Manager::Get_ChangeFormation_Cooltime()
{
	return change_formation_ct;
}

high_resolution_clock::time_point CServer_Manager::Get_AddNPC_Cooltime()
{
	return add_npc_ct;
}

void CServer_Manager::Set_AddNPC_CoolTime(high_resolution_clock::time_point ct)
{
	add_npc_ct = ct;
}

void CServer_Manager::Set_ChangeFormation_CoolTime(high_resolution_clock::time_point ct)
{
	change_formation_ct = ct;
}

void CServer_Manager::Set_wParam(WPARAM p)
{
	m_wparam = p;
}
