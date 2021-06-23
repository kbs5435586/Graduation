#include "framework.h"
#include "Server_Manager.h"
#include "Management.h"
#include "Layer.h"
#include "MyRect.h"

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
	inet_pton(AF_INET, SERVER_IP.c_str(), &server_a.sin_addr); //
	server_a.sin_port = htons(SERVER_PORT);

	init_client();

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
		short recv_id = my_packet->id;
		my_id = recv_id;
		my_hp = my_packet->hp;

		CTransform* pTransform;
		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Player", L"Com_Transform", recv_id);

		_matrix Pos = pTransform->Get_Matrix();
		Pos._11 = my_packet->r_x;
		Pos._12 = my_packet->r_y;
		Pos._13 = my_packet->r_z;
		Pos._21 = my_packet->u_x;
		Pos._22 = my_packet->u_y;
		Pos._23 = my_packet->u_z;
		Pos._31 = my_packet->l_x;
		Pos._32 = my_packet->l_y;
		Pos._33 = my_packet->l_z;
		Pos._41 = my_packet->p_x;
		Pos._42 = my_packet->p_y;
		Pos._43 = my_packet->p_z;

		m_objects[recv_id].showObject = true;
		m_objects[recv_id].anim = 14;
		m_objects[recv_id].hp = my_packet->hp;
		pTransform->Set_Matrix(Pos);
		add_npc_ct = high_resolution_clock::now(); // 임시 NPC 소환 쿨타임 초기화
		change_formation_ct = high_resolution_clock::now(); // 임시 NPC 소환 쿨타임 초기화
		isLogin = true;

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

		if (is_player(recv_id)) // 플레이어 일때
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_Player", L"Com_Transform", recv_id);
		}
		else if (is_npc(recv_id)) // NPC 일때
		{
			short npc_id = npc_id_to_idx(recv_id);
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_NPC", L"Com_Transform", npc_id);
		}
		else // 환경요소
		{

		}
		_matrix Pos = pTransform->Get_Matrix();
		strcpy_s(m_objects[recv_id].name, my_packet->name);
		Pos._11 = my_packet->r_x;
		Pos._12 = my_packet->r_y;
		Pos._13 = my_packet->r_z;
		Pos._21 = my_packet->u_x;
		Pos._22 = my_packet->u_y;
		Pos._23 = my_packet->u_z;
		Pos._31 = my_packet->l_x;
		Pos._32 = my_packet->l_y;
		Pos._33 = my_packet->l_z;
		Pos._41 = my_packet->p_x;
		Pos._42 = my_packet->p_y;
		Pos._43 = my_packet->p_z;
		m_objects[recv_id].showObject = true;
		m_objects[recv_id].anim = 14;
		m_objects[recv_id].hp = my_packet->hp;
		pTransform->Set_Matrix(Pos);
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
		if (is_player(recv_id)) // 플레이어
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_Player", L"Com_Transform", recv_id);
		}
		else if (is_npc(recv_id))
		{
			short npc_id = npc_id_to_idx(recv_id);
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_NPC", L"Com_Transform", npc_id);
		}
		m_objects[recv_id].anim = 29;
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

		if (is_player(recv_id)) // 플레이어
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_Player", L"Com_Transform", recv_id);
		}
		else if (is_npc(recv_id))
		{
			short npc_id = npc_id_to_idx(recv_id);
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_NPC", L"Com_Transform", npc_id);
		}
		m_objects[recv_id].anim = 29;
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

		if (0 != m_objects.count(other_id))
			m_objects[other_id].showObject = false;
	}
	break;
	case SC_PACKET_ADD_NPC_OK:
	{
		sc_packet_npc_add_ok* my_packet = reinterpret_cast<sc_packet_npc_add_ok*>(ptr);
		int npc_id = my_packet->id;
	}
	break;
	case SC_PACKET_IDLE:
	{
		sc_packet_idle* my_packet = reinterpret_cast<sc_packet_idle*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].anim = 14;
	}
	break;
	case SC_PACKET_ATTACK:
	{
		sc_packet_attack* my_packet = reinterpret_cast<sc_packet_attack*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].anim = 0;
	}
	break;
	case SC_PACKET_ATTACKED:
	{
		sc_packet_attacked* my_packet = reinterpret_cast<sc_packet_attacked*>(ptr);
		int recv_id = my_packet->id;
		short recv_hp = my_packet->hp;
		m_objects[recv_id].hp = recv_hp;
		m_objects[recv_id].anim = 12;
	}
	break;
	case SC_PACKET_DEAD:
	{
		sc_packet_dead* my_packet = reinterpret_cast<sc_packet_dead*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].hp = 0;
		m_objects[recv_id].anim = 8;

		if (is_player(recv_id))
		for (int i = MY_NPC_START(recv_id); i <= MY_NPC_END(recv_id); ++i)
		{
			if (0 != m_objects.count(i) /*&& m_objects[i].showObject*/)
				m_objects[recv_id].anim = 8;
		}

		//managment = CManagement::GetInstance();  // 플레이어의 죽는 애니메이션이 끝났을때 실행되게
		//if (nullptr == managment)
		//	return;
		//managment->AddRef();

		//CTransform* pTransform;
		//if (0 == m_objects[my_id].hp)
		//{
		//	if (ENUM_PLAYER1 == my_id)
		//	{
		//		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		//			L"Layer_Orc02", L"Com_Transform", 0);
		//		_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
		//		vPos.x = 50.f;
		//		vPos.y = 0.2f;
		//		vPos.z = 90.f;
		//		pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		//		send_position_packet(&vPos);

		//	}
		//	else if (ENUM_PLAYER2 == my_id) // 다른 플레이어
		//	{
		//		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		//			L"Layer_Orc04", L"Com_Transform", 0);
		//		_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
		//		vPos.x = 450.f;
		//		vPos.y = 0.2f;
		//		vPos.z = 360.f;
		//		pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		//		send_position_packet(&vPos);
		//	}
		//}

		//for (int i = MY_NPC_START(other_id); i <= MY_NPC_END(other_id); ++i)
		//{
		//	if (0 != m_objects.count(i))
		//		m_objects[i].showObject = false;
		//}

		//Safe_Release(managment);
	}
	break;
	case SC_PACKET_FLAG_INFO:
	{
		sc_packet_flag_info* my_packet = reinterpret_cast<sc_packet_flag_info*>(ptr);
		int recv_id = my_packet->id;

		CTransform* pTransform;
		_vec3 vPos;

		flags[recv_id].isBlue = my_packet->isBlue;
		flags[recv_id].isRed = my_packet->isRed;

		vPos.x = my_packet->p_x;
		vPos.y = my_packet->p_y;
		vPos.z = my_packet->p_z;

		int iTemp = 0;

		/*for (auto& iter : managment->Get_Layer((_uint)SCENEID::SCENE_STAGE, L"Layer_Rect")->Get_GameObjectLst())
		{
			dynamic_cast<CMyRect*>(iter)->m_iLayerIdx = iTemp;
			iTemp++;
		}*/

		//pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		//	L"Layer_Flag", L"Com_Transform", recv_id);
		//pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);

		//pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
		//	L"Layer_Rect", L"Com_Transform", recv_id);
		//pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
	}
	break;
	case SC_PACKET_FLAG_BOOL:
	{
		sc_packet_flag_bool* my_packet = reinterpret_cast<sc_packet_flag_bool*>(ptr);
		int recv_id = my_packet->id;

		flags[recv_id].isBlue = my_packet->isBlue;
		flags[recv_id].isRed = my_packet->isRed;
	}
	break;
	case SC_PACKET_TIME:
	{
		sc_packet_time* my_packet = reinterpret_cast<sc_packet_time*>(ptr);
		game_time = my_packet->time;
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

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
	{
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

void CServer_Manager::send_attack_packet()
{
	cs_packet_attack m_packet;
	m_packet.type = CS_PACKET_ATTACK;
	m_packet.size = sizeof(m_packet);
	send_packet(&m_packet);
}

void CServer_Manager::send_position_packet(_vec3* pos)
{
	cs_packet_position m_packet;
	m_packet.type = CS_PACKET_POSITION;
	m_packet.size = sizeof(m_packet);
	m_packet.x = pos->x;
	m_packet.y = pos->y;
	m_packet.z = pos->z;
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
	//strcpy_s(m_objects.name, l_packet.name); // 닉네임 이름
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

void CServer_Manager::send_idle_packet()
{
	cs_packet_idle l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_IDLE;
	send_packet(&l_packet);
}

void CServer_Manager::update_key_input()
{
	/*if ((GetAsyncKeyState('1') & 0x8000))
	{
		send_npc_act_packet(DO_ATTACK);
		isSendOnePacket = false;
	}
	if ((GetAsyncKeyState('2') & 0x8000))
	{
		send_npc_act_packet(DO_DEFENCE);
		isSendOnePacket = false;
	}
	if ((GetAsyncKeyState('3') & 0x8000))
	{
		send_npc_act_packet(DO_HOLD);
		isSendOnePacket = false;
	}
	if ((GetAsyncKeyState('4') & 0x8000))
	{
		send_npc_act_packet(DO_FOLLOW);
		isSendOnePacket = false;
	}
	if ((GetAsyncKeyState('5') & 0x8000))
	{
		send_npc_act_packet(DO_RANDMOVE);
		isSendOnePacket = false;
	}*/


	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		duration<double> cool_time = duration_cast<duration<double>>(high_resolution_clock::now()
			- Get_Attack_Cooltime());
		if (cool_time.count() > 2) // ↑ 쿨타임 2초 계산해주는 식
		{
			send_attack_packet();
			m_objects[my_id].anim = 0;
			isSendOnePacket = false;
			Set_Attack_CoolTime(high_resolution_clock::now());
		}
	}
	else
	{
		
	}

	//if (true == isLogin)
	//{
	//	if (!(GetAsyncKeyState('T') & 0x8000) && !(GetAsyncKeyState('F') & 0x8000) && !(GetAsyncKeyState('G') & 0x8000) &&
	//		!(GetAsyncKeyState('H') & 0x8000) && !(GetAsyncKeyState('O') & 0x8000) && !(GetAsyncKeyState('P') & 0x8000) &&
	//		!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	//	{
	//		if (false == isSendOnePacket)
	//		{
	//			send_idle_packet();
	//			isSendOnePacket = true;
	//			m_objects[my_id].anim = 14;
	//		}
	//	}
	//}
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
	return id + 30;
}

short CServer_Manager::npc_id_to_idx(unsigned short id)
{
	return id - 30;
}

short CServer_Manager::object_idx_to_id(unsigned short id)
{
	return id + 450;
}

short CServer_Manager::object_id_to_idx(unsigned short id)
{
	return id - 450;
}


void CServer_Manager::send_npc_act_packet(unsigned char act)
{
	if (my_id < NPC_START)
	{
		cs_packet_npc_act l_packet;
		l_packet.size = sizeof(l_packet);
		l_packet.type = CS_PACKET_NPC_ACT;
		l_packet.id = my_id;
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
	return m_objects[my_id].showObject;
}

bool CServer_Manager::Get_ShowNPC(int index)
{
	short npc_index = npc_idx_to_id(index);
	return m_objects[npc_index].showObject;
}

bool CServer_Manager::Get_Blue(int id)
{
	return flags[id].isBlue;
}

bool CServer_Manager::Get_Red(int id)
{
	return flags[id].isRed;
}

bool CServer_Manager::Get_Login()
{
	return isLogin;
}

short CServer_Manager::Get_PlayerID()
{
	return my_id;
}

short CServer_Manager::Get_PlayerHP(int id)
{
	return m_objects[id].hp;
}

short CServer_Manager::Get_NpcHP(int id)
{
	short npc_index = npc_idx_to_id(id);
	return m_objects[npc_index].hp;
}

short CServer_Manager::Get_ShowOtherPlayer(int id)
{
	return m_objects[id].showObject;
}

short CServer_Manager::Get_Anim(int id)
{
	return m_objects[id].anim;
}

float CServer_Manager::Get_GameTime()
{
	return game_time;
}

bool CServer_Manager::is_player(int id)
{
	if (id < NPC_START)
		return true;
	else
		return false;
}

bool CServer_Manager::is_npc(int id)
{
	if (id >= NPC_START && id < OBJECT_START)
		return true;
	else
		return false;
}

bool CServer_Manager::is_object(int id)
{
	if (id >= OBJECT_START)
		return true;
	else
		return false;
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

high_resolution_clock::time_point CServer_Manager::Get_Attack_Cooltime()
{
	return attack_ct;
}

void CServer_Manager::Set_Attack_CoolTime(high_resolution_clock::time_point ct)
{
	attack_ct = ct;
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

void CServer_Manager::init_client()
{
	isRed = false;
	isBlue = false;
}
