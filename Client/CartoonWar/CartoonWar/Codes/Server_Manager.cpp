#include "framework.h"
#include "Server_Manager.h"
#include "Management.h"
#include "Layer.h"
#include "MyRect.h"
#include "Fire.h"

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

	SOCKADDR_IN server_a;
	ZeroMemory(&server_a, sizeof(server_a));
	server_a.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.218.130", &server_a.sin_addr); //SERVER_IP.c_str() // 192.168.218.157 / 255.255.255.0 / 192.168.218.181
	server_a.sin_port = htons(SERVER_PORT);

	init_client();

	int retval = connect(m_cSocket, (SOCKADDR*)&server_a, sizeof(server_a));
	if ((retval == SOCKET_ERROR) && (WSAEWOULDBLOCK != WSAGetLastError())) // 비동기 connect는 바로 리턴되면서 WSAEWOULDBLOCK 에러를 발생시킴
	{
		Free();
		err_quit("connect Error\n");
		return FALSE;
	}
	else
	{
		isConnected = true;
		send_login_ok_packet();
	}

	m_EventArray = WSACreateEvent();
	retval = WSAEventSelect(m_cSocket, m_EventArray, FD_READ | FD_CLOSE);
	if (retval == SOCKET_ERROR)
	{
		Free();
		err_quit("WSAAsyncSelect Error\n");
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
		g_iPlayerIdx = my_id;
		my_hp = my_packet->hp;
		my_npc = 0; // 수정
		my_troop = T_ALL;
		my_last_troop = T_ALL;


		CTransform* pTransform;
		pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
			L"Layer_Player", L"Com_Transform", recv_id);

		_matrix mat;
		mat._11 = my_packet->r_x;
		mat._12 = my_packet->r_y;
		mat._13 = my_packet->r_z;
		mat._21 = my_packet->u_x;
		mat._22 = my_packet->u_y;
		mat._23 = my_packet->u_z;
		mat._31 = my_packet->l_x;
		mat._32 = my_packet->l_y;
		mat._33 = my_packet->l_z;
		mat._41 = my_packet->p_x;
		mat._42 = my_packet->p_y;
		mat._43 = my_packet->p_z;
		pTransform->Set_Matrix(mat);

		m_objects[recv_id].showObject = true;
		m_objects[recv_id].anim = 0;
		m_objects[recv_id].hp = my_packet->hp;
		m_objects[recv_id].m_class = (int)my_packet->p_class;

		add_npc_ct = high_resolution_clock::now(); // 임시 NPC 소환 쿨타임 초기화
		change_formation_ct = high_resolution_clock::now(); // 임시 NPC 소환 쿨타임 초기화
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
		_matrix Pos;
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
		pTransform->Set_Matrix(Pos);

		m_objects[recv_id].showObject = true;
		m_objects[recv_id].hp = my_packet->hp;
		m_objects[recv_id].con_move = my_packet->con_move;
		m_objects[recv_id].con_rotate = my_packet->con_rotate;
		m_objects[recv_id].m_class = my_packet->p_class;

		Safe_Release(managment);
	}
	break;
	case SC_PACKET_CONDITION:
	{
		sc_packet_condition* my_packet = reinterpret_cast<sc_packet_condition*>(ptr);
		int recv_id = my_packet->id;
		char con = my_packet->condition;

		if (CON_TYPE_MOVE == my_packet->con_type)
			m_objects[recv_id].con_move = con;
		else if (CON_TYPE_ROTATE == my_packet->con_type)
			m_objects[recv_id].con_rotate = con;
	}
	break;
	case SC_PACKET_NPC_SIZE:
	{
		sc_packet_npc_size* my_packet = reinterpret_cast<sc_packet_npc_size*>(ptr);
		my_npc = my_packet->npc_size;
	}
	break;
	case SC_PACKET_FIRE:
	{
		sc_packet_fire* my_packet = reinterpret_cast<sc_packet_fire*>(ptr);
		XMFLOAT2 fTemp = { my_packet->x, my_packet->z };
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Fire", (_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", nullptr, (void*)&fTemp)))
			return;

		list<CGameObject*> lst = CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire");
		int numver = lst.size();
		CGameObject* fire = CManagement::GetInstance()->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", numver - 1);
		dynamic_cast<CFire*>(fire)->SetSCheck(true);
	}
	break;
	case SC_PACKET_TELEPORT:
	{
		sc_packet_teleport* my_packet = reinterpret_cast<sc_packet_teleport*>(ptr);

		if (M_ADD == my_packet->mode)
		{
			XMFLOAT2 fTemp = { my_packet->x,my_packet->z };
			if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Teleport", (_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport", nullptr, (void*)&fTemp)))
				return;
		}
		else
		{
			for (auto& iter0 : CManagement::GetInstance()->Get_GameObjectLst((_uint)SCENEID::SCENE_STAGE, L"Layer_SkillTeleport"))
			{
				_vec3 iter0_Pos = *dynamic_cast<CTransform*>(iter0->Get_ComponentPointer(L"Com_Transform"))->Get_StateInfo(CTransform::STATE_POSITION);
				if ((iter0_Pos.x == my_packet->x) && (iter0_Pos.y == my_packet->z))
				{
					iter0->GetIsDead();
				}
			}
		}
	}
	break;
	case SC_PACKET_HIT:
	{
		sc_packet_hit* my_packet = reinterpret_cast<sc_packet_hit*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].isHit = my_packet->ishit;
	}
	break;
	case SC_PACKET_INVISIBLE:
	{
		sc_packet_invisible* my_packet = reinterpret_cast<sc_packet_invisible*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].isInvisible = my_packet->invisable;
	}
	break;
	case SC_PACKET_FIX:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();
		CTransform* pTransform;

		sc_packet_fix* my_packet = reinterpret_cast<sc_packet_fix*>(ptr);
		int recv_id = my_packet->id;
		if (is_player(recv_id))
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
		_vec3* vPos = pTransform->Get_StateInfo(CTransform::STATE_POSITION);
		_vec3 rPos, uPos, lPos;
		rPos.x = my_packet->r_x, rPos.y = my_packet->r_y, rPos.z = my_packet->r_z;
		uPos.x = my_packet->u_x, uPos.y = my_packet->u_y, uPos.z = my_packet->u_z;
		lPos.x = my_packet->l_x, lPos.y = my_packet->l_y, lPos.z = my_packet->l_z;
		vPos->x = my_packet->p_x; vPos->z = my_packet->p_z;
		pTransform->Set_StateInfo(CTransform::STATE_RIGHT, &rPos);
		pTransform->Set_StateInfo(CTransform::STATE_UP, &uPos);
		pTransform->Set_StateInfo(CTransform::STATE_LOOK, &lPos);
		pTransform->Set_StateInfo(CTransform::STATE_POSITION, vPos);
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
	case SC_PACKET_ANIMATION:
	{
		sc_packet_animation* my_packet = reinterpret_cast<sc_packet_animation*>(ptr);
		update_anim(my_packet->id, my_packet->anim);
	}
	break;
	case SC_PACKET_ATTACKED:
	{
		sc_packet_attacked* my_packet = reinterpret_cast<sc_packet_attacked*>(ptr);
		int recv_id = my_packet->id;
		short recv_hp = my_packet->hp;
		m_objects[recv_id].hp = recv_hp;
	}
	break;
	case SC_PACKET_DEAD:
	{
		sc_packet_dead* my_packet = reinterpret_cast<sc_packet_dead*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].hp = my_packet->hp;
		update_anim(recv_id, my_packet->anim);
		//m_objects[recv_id].showObject = false;

		//managment = CManagement::GetInstance();  // 플레이어의 죽는 애니메이션이 끝났을때 실행되게
		//if (nullptr == managment)
		//	return;
		//managment->AddRef();

		//CTransform* pTransform;
		//if (0 == m_objects[my_id].hp)
		//{
		//	pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", L"Com_Transform", my_id);
		//	_vec3 vPos = *pTransform->Get_StateInfo(CTransform::STATE_POSITION);
		//	if (0 == my_id)
		//	{
		//		vPos.x = 50.f;
		//		vPos.y = 0.f;
		//		vPos.z = 90.f;
		//	}
		//	else
		//	{
		//		vPos.x = 90.f;
		//		vPos.y = 0.f;
		//		vPos.z = 30.f;
		//		_vec3 pos = { 90.f, 0.f, 30.f };
		//	}
		//	pTransform->Set_StateInfo(CTransform::STATE_POSITION, &vPos);
		//	send_position_packet(&vPos);
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
	case SC_PACKET_CLASS_CHANGE:
	{
		sc_packet_class_change* my_packet = reinterpret_cast<sc_packet_class_change*>(ptr);
		m_objects[my_packet->id].m_class = my_packet->p_class;
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

HRESULT CServer_Manager::EventManager()
{
	if (isConnected)
	{
		int index = WSAWaitForMultipleEvents(1, &m_EventArray, FALSE, 0, FALSE);
		if (index == WSA_WAIT_FAILED)
			return E_FAIL;

		index -= WSA_WAIT_EVENT_0;
		WSANETWORKEVENTS NetworkEvents;

		int retval = WSAEnumNetworkEvents(m_cSocket, m_EventArray, &NetworkEvents);
		if (retval == SOCKET_ERROR)
			return E_FAIL;

		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			if (NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
				return E_FAIL;

			char net_buf[MAX_BUF_SIZE];
			auto recv_result = recv(m_cSocket, net_buf, MAX_BUF_SIZE, 0);
			if (recv_result == SOCKET_ERROR)
				return E_FAIL;
			else if (recv_result == 0)
				return E_FAIL;

			if (recv_result > 0)
				process_data(net_buf, recv_result);
		}
		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			if (NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
				return E_FAIL;

			Free();
		}
	}
}

void CServer_Manager::send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	//size_t sent;
	send(m_cSocket, p, p[0], 0);
	//g_socket.send(p, p[0], sent);
}

void CServer_Manager::send_condition_packet(unsigned char con_type, unsigned char con)
{
	cs_packet_condition m_packet;
	m_packet.type = CS_PACKET_CONDITION;
	m_packet.size = sizeof(m_packet);
	m_packet.con_type = con_type;
	m_packet.con = con;
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

void CServer_Manager::send_change_troop()
{
	cs_packet_change_troop l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_TROOP_CHANGE;
	l_packet.troop = my_troop;
	send_packet(&l_packet);
}

void CServer_Manager::send_class_change_packet(int idx, char type)
{
	cs_packet_class_change l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_CLASS_CHANGE;
	if (type == O_PLAYER)
	{
		l_packet.id = idx;
	}
	else if (type == O_NPC)
	{
		l_packet.id = npc_idx_to_id(idx);
	}
	l_packet.p_class = m_objects[l_packet.id].m_class;
	send_packet(&l_packet);
}

void CServer_Manager::send_change_formation_packet()
{
	cs_packet_change_formation l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_CHANGE_FORMATION;
	send_packet(&l_packet);
}

void CServer_Manager::send_arrow_packet()
{
	cs_packet_arrow l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_ARROW;
	send_packet(&l_packet);
}

void CServer_Manager::send_invisible_packet(bool isinvi)
{
	cs_packet_invisible l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_INVISIBLE;
	l_packet.invisable = isinvi;
	send_packet(&l_packet);
}

void CServer_Manager::send_animation_packet(unsigned char anim)
{
	cs_packet_animation l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_ANIMATION;
	l_packet.anim = anim;
	send_packet(&l_packet);
}

void CServer_Manager::send_fire_packet(float mx, float mz)
{
	cs_packet_fire l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_FIRE;
	l_packet.x = mx;
	l_packet.z = mz;
	send_packet(&l_packet);
}

void CServer_Manager::send_teleport_packet(float mx, float mz)
{
	cs_packet_teleport l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_TELEPORT;
	l_packet.x = mx;
	l_packet.z = mz;
	send_packet(&l_packet);
}

void CServer_Manager::update_key_input()
{
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		disconnect();
		PostQuitMessage(0);
	}
}

void CServer_Manager::update_anim(int id, unsigned char anim)
{
	switch (m_objects[id].m_class)
	{
	case C_WORKER:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 0;
		if (A_CWALK == anim)
			m_objects[id].anim = 1;
		if (A_ATTACK == anim)
			m_objects[id].anim = 3;
		if (A_HIT == anim)
			m_objects[id].anim = 6;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 2;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 4;
			else
				m_objects[id].anim = 5;
		}
		break;
	case C_CAVALRY:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 4;
		if (A_CWALK == anim)
			m_objects[id].anim = 5;
		if (A_ATTACK == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 6;
			else
				m_objects[id].anim = 7;
		}
		if (A_HIT == anim)
			m_objects[id].anim = 8;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 3;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 9;
			else
				m_objects[id].anim = 10;
		}
		break;
	case C_TWO:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 4;
		if (A_CWALK == anim)
			m_objects[id].anim = 5;
		if (A_ATTACK == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 6;
			else
				m_objects[id].anim = 7;
		}
		if (A_HIT == anim)
			m_objects[id].anim = 8;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 3;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 9;
			else
				m_objects[id].anim = 10;
		}
		break;
	case C_INFANTRY:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 4;
		if (A_CWALK == anim)
			m_objects[id].anim = 5;
		if (A_ATTACK == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 6;
			else
				m_objects[id].anim = 7;
		}
		if (A_HIT == anim)
			m_objects[id].anim = 8;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 3;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 9;
			else
				m_objects[id].anim = 10;
		}
		break;
	case C_FOUR:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 4;
		if (A_CWALK == anim)
			m_objects[id].anim = 5;
		if (A_ATTACK == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 6;
			else
				m_objects[id].anim = 7;
		}
		if (A_HIT == anim)
			m_objects[id].anim = 8;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 3;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 9;
			else
				m_objects[id].anim = 10;
		}
	case C_SPEARMAN:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 4;
		if (A_CWALK == anim)
			m_objects[id].anim = 5;
		if (A_ATTACK == anim)
			m_objects[id].anim = 6;
		if (A_HIT == anim)
			m_objects[id].anim = 7;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 3;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 8;
			else
				m_objects[id].anim = 9;
		}
		break;
	case C_MAGE:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 4;
		if (A_CWALK == anim)
			m_objects[id].anim = 5;
		if (A_ATTACK == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 6;
			else
				m_objects[id].anim = 7;
		}
		if (A_HIT == anim)
			m_objects[id].anim = 8;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 3;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 9;
			else
				m_objects[id].anim = 10;
		}
		if (A_CAST == anim)
		{
			int i = rand() % 3;
			if (i == 0)
				m_objects[id].anim = 11;
			else if(i == 1)
				m_objects[id].anim = 12;
			else
				m_objects[id].anim = 13;
		}
		break;
	case C_MMAGE:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 0;
		if (A_CWALK == anim)
			m_objects[id].anim = 1;
		if (A_ATTACK == anim)
			m_objects[id].anim = 3;
		if (A_HIT == anim)
			m_objects[id].anim = 4;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 2;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 5;
			else
				m_objects[id].anim = 6;
		}
		if (A_CAST == anim)
		{
			int i = rand() % 3;
			if (i == 0)
				m_objects[id].anim = 7;
			else if (i == 1)
				m_objects[id].anim = 8;
			else
				m_objects[id].anim = 9;
		}
		break;
	case C_ARCHER:
		if (A_IDLE == anim)
			m_objects[id].anim = 0;
		if (A_WALK == anim)
			m_objects[id].anim = 1;
		if (A_CIDLE == anim)
			m_objects[id].anim = 3;
		if (A_CWALK == anim)
			m_objects[id].anim = 4;
		if (A_ATTACK == anim)
			m_objects[id].anim = 5;
		if (A_HIT == anim)
			m_objects[id].anim = 6;
		if (A_RUN == anim)
			m_objects[id].anim = 2;
		if (A_CRUN == anim)
			m_objects[id].anim = 2;
		if (A_DEAD == anim)
		{
			int i = rand() % 2;
			if (i == 0)
				m_objects[id].anim = 7;
			else
				m_objects[id].anim = 8;
		}
		break;
	}
}

short CServer_Manager::npc_idx_to_id(unsigned short id)
{
	return id + NPC_START;
}

short CServer_Manager::npc_id_to_idx(unsigned short id)
{
	return id - NPC_START;
}

short CServer_Manager::object_idx_to_id(unsigned short id)
{
	return id + OBJECT_START;
}

short CServer_Manager::object_id_to_idx(unsigned short id)
{
	return id - OBJECT_START;
}


void CServer_Manager::send_npc_act_packet(unsigned char act)
{
	if (my_id < NPC_START)
	{
		cs_packet_npc_act l_packet;
		l_packet.size = sizeof(l_packet);
		l_packet.type = CS_PACKET_NPC_ACT;
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

bool CServer_Manager::Get_Connected()
{
	return isConnected;
}

short CServer_Manager::Get_PlayerID()
{
	return my_id;
}

short CServer_Manager::Get_PlayerHP(int id)
{
	return m_objects[id].hp;
}

char CServer_Manager::Get_PlayerMCon(int id)
{
	return m_objects[id].con_move;
}

char CServer_Manager::Get_PlayerRCon(int id)
{
	return m_objects[id].con_rotate;
}

int CServer_Manager::Get_PlayerClass(int id)
{
	return m_objects[id].m_class;
}

void CServer_Manager::Set_Class(int mclass, int id, char type)
{
	if (type == O_PLAYER)
		m_objects[id].m_class = mclass;
	else if (type == O_NPC)
		m_objects[npc_idx_to_id(id)].m_class = mclass;
}

int CServer_Manager::Get_NpcClass(int id)
{
	short npc_index = npc_idx_to_id(id);
	return m_objects[npc_index].m_class;
}

char CServer_Manager::Get_NpcMCon(int id)
{
	short npc_index = npc_idx_to_id(id);
	return m_objects[npc_index].con_move;
}

char CServer_Manager::Get_NpcRCon(int id)
{
	short npc_index = npc_idx_to_id(id);
	return m_objects[npc_index].con_rotate;
}

void CServer_Manager::Set_PlayerMCon(char cond)
{
	m_objects[my_id].con_move = cond;
}

void CServer_Manager::Set_PlayerRCon(char cond)
{
	m_objects[my_id].con_rotate = cond;
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

short CServer_Manager::Get_AnimNPC(int id)
{
	int npc_id = npc_idx_to_id(id);
	return m_objects[npc_id].anim;
}

void CServer_Manager::Set_AnimPL(int id, short anim)
{
	m_objects[id].anim = anim;
}

void CServer_Manager::Set_AnimNPC(int id, short anim)
{
	m_objects[npc_idx_to_id(id)].anim = anim;
}

bool CServer_Manager::Get_isHitPL(int id)
{
	return m_objects[id].isHit;
}

bool CServer_Manager::Get_isInvisible(int id)
{
	return m_objects[id].isInvisible;
}

bool CServer_Manager::Get_isHitNPC(int id)
{
	int npc_id = npc_idx_to_id(id);
	return m_objects[npc_id].isHit;
}

void CServer_Manager::Set_isHitPL(int id, bool ishit)
{
	m_objects[id].isHit = ishit;
}

void CServer_Manager::Set_isHitNPC(int id, bool ishit)
{
	m_objects[npc_idx_to_id(id)].isHit = ishit;
}

short CServer_Manager::Get_NpcSize()
{
	return my_npc;
}

short CServer_Manager::Get_TroopClass()
{
	return my_troop;
}

void CServer_Manager::Set_TroopClass(short mclass)
{
	my_troop = mclass;
	if (my_last_troop != my_troop)
	{
		my_last_troop = my_troop;
		send_change_troop();
	}
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

void CServer_Manager::Set_Anim(unsigned short anim)
{
	m_objects[my_id].anim = anim;
}

void CServer_Manager::init_client()
{
	isRed = false;
	isBlue = false;
}
