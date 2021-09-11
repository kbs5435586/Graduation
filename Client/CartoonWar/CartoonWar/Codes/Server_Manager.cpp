#include "framework.h"
#include "Server_Manager.h"
#include "Management.h"
#include "Layer.h"
#include "MyRect.h"
#include "Fire.h"
#include <iostream>

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
	inet_pton(AF_INET, "192.168.208.130", &server_a.sin_addr); //SERVER_IP.c_str() // 218.49.11.188 / 218.49.11.188 / 1.241.121.48
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

		_vec3 pos = { mat._41, mat._42, mat._43 };

		CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
		if (nullptr == pTerrainBuffer)
			return;
		_float fY = pTerrainBuffer->Compute_HeightOnTerrain(&pos);
		mat._42 = fY;

		pTransform->Set_Matrix(mat);
		Set_Server_Mat(recv_id, &mat);
		m_objects[recv_id].showObject = true;
		m_objects[recv_id].isFirst = true;
		m_objects[recv_id].anim = 0;
		m_objects[recv_id].hp = my_packet->hp;
		m_objects[recv_id].m_class = (int)my_packet->p_class;

		add_npc_ct = high_resolution_clock::now();
		change_formation_ct = high_resolution_clock::now();
		attack_ct = high_resolution_clock::now();
		move_ct = high_resolution_clock::now();
		rotate_ct = high_resolution_clock::now();
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

		_matrix mat;
		strcpy_s(m_objects[recv_id].name, my_packet->name);
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
		_vec3 pos = { mat._41, mat._42, mat._43 };

		CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
		if (nullptr == pTerrainBuffer)
			return;
		_float fY = pTerrainBuffer->Compute_HeightOnTerrain(&pos);
		mat._42 = fY;

		if (is_player(recv_id)) // 플레이어 일때
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_Player", L"Com_Transform", recv_id);
			pTransform->Set_Matrix(mat);
			Set_Server_Mat(recv_id, &mat);
		}
		else if (is_npc(recv_id)) // NPC 일때 // 화살 수정

		{
			short npc_id = npc_id_to_idx(recv_id);
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_NPC", L"Com_Transform", npc_id);
			pTransform->Set_Matrix(mat);
			Set_Server_Mat(recv_id, &mat);
		}
		else // 환경요소
		{

		}

		m_objects[recv_id].showObject = true;
		m_objects[recv_id].isFirst = true;
		m_objects[recv_id].anim_stat = A_IDLE;
		m_objects[recv_id].hp = my_packet->hp;
		m_objects[recv_id].m_class = my_packet->p_class;

		Safe_Release(managment);
	}
	break;
	case SC_PACKET_REVIVE:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();

		sc_packet_revive* my_packet = reinterpret_cast<sc_packet_revive*>(ptr);
		int recv_id = my_packet->id;
		CTransform* pTransform;

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
		_vec3 pos = { mat._41, mat._42, mat._43 };

		CBuffer_Terrain_Height* pTerrainBuffer = (CBuffer_Terrain_Height*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE, L"Layer_Terrain", L"Com_Buffer");
		if (nullptr == pTerrainBuffer)
			return;
		_float fY = pTerrainBuffer->Compute_HeightOnTerrain(&pos);
		mat._42 = fY;

		if (is_player(recv_id)) // 플레이어 일때
		{
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_Player", L"Com_Transform", recv_id);
			pTransform->Set_Matrix(mat);
			Set_Server_Mat(recv_id, &mat);
		}
		else if (is_npc(recv_id)) // NPC 일때 // 화살 수정

		{
			short npc_id = npc_id_to_idx(recv_id);
			pTransform = (CTransform*)managment->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
				L"Layer_NPC", L"Com_Transform", npc_id);
			pTransform->Set_Matrix(mat);
			Set_Server_Mat(recv_id, &mat);
		}

		m_objects[recv_id].showObject = true;
		m_objects[recv_id].isFirst = true;
		m_objects[recv_id].hp = my_packet->hp;
		m_objects[recv_id].anim = my_packet->anim;
		m_objects[recv_id].anim_stat = A_IDLE;
		update_anim(my_packet->id, my_packet->anim);
		Safe_Release(managment);
	}
	break;
	case SC_PACKET_NPC_SIZE:
	{
		sc_packet_npc_size* my_packet = reinterpret_cast<sc_packet_npc_size*>(ptr);
		my_npc = my_packet->npc_size;
	}
	break;
	case SC_PACKET_NATURE_SCALE:
	{
		sc_packet_nature_scale* my_packet = reinterpret_cast<sc_packet_nature_scale*>(ptr);
		m_objects[my_packet->id].scale = my_packet->scale;
	}
	break;
	case SC_PACKET_FIRE:
	{
		sc_packet_fire* my_packet = reinterpret_cast<sc_packet_fire*>(ptr);
		XMFLOAT2 fTemp = { my_packet->x, my_packet->z };
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Fire", (_uint)SCENEID::SCENE_STAGE, L"Layer_SkillFire", nullptr, (void*)&fTemp)))
			return;
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
	case SC_PACKET_HP:
	{
		sc_packet_hp* my_packet = reinterpret_cast<sc_packet_hp*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].hp = my_packet->hp;
	}
	break;
	case SC_PACKET_DO_PARTICLE:
	{
		sc_packet_do_particle* my_packet = reinterpret_cast<sc_packet_do_particle*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].isParticle = true;
	}
	break;
	case SC_PACKET_INVISIBLE:
	{
		sc_packet_invisible* my_packet = reinterpret_cast<sc_packet_invisible*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].isInvisible = my_packet->invisable;
	}
	break;
	case SC_PACKET_MOVE:
	{
		managment = CManagement::GetInstance();
		if (nullptr == managment)
			return;
		managment->AddRef();
		CTransform* pTransform;

		sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
		int recv_id = my_packet->id;

		_matrix temp;

		temp._11 = my_packet->r_x;
		temp._12 = my_packet->r_y;
		temp._13 = my_packet->r_z;
		temp._21 = my_packet->u_x;
		temp._22 = my_packet->u_y;
		temp._23 = my_packet->u_z;
		temp._31 = my_packet->l_x;
		temp._32 = my_packet->l_y;
		temp._33 = my_packet->l_z;
		temp._41 = my_packet->p_x;
		if (!is_object(recv_id))
			temp._42 = my_packet->p_y;
		temp._43 = my_packet->p_z;

		Set_Server_Mat(recv_id, &temp);
		Safe_Release(managment);
	}
	break;
	case SC_PACKET_LEAVE:
	{
		sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
		int other_id = my_packet->id;
		m_objects[other_id].showObject = false;
	}
	break;
	case SC_PACKET_GOLD:
	{
		sc_packet_gold* my_packet = reinterpret_cast<sc_packet_gold*>(ptr);
		g_iGold = my_packet->gold;
	}
	break;
	case SC_PACKET_ARROW:
	{
		sc_packet_arrow* my_packet = reinterpret_cast<sc_packet_arrow*>(ptr);
		int recv_id = my_packet->shoot_id;
		int arrow_idx = object_id_to_idx(my_packet->arrow_id);
;
		CTransform* pTransform = nullptr;
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
		_matrix matTemp = pTransform->Get_Matrix();
		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_ThrowArrow", (_uint)SCENEID::SCENE_STAGE, L"Layer_Arrow", nullptr, (void*)&matTemp, arrow_idx)))
			return;
		m_objects[my_packet->arrow_id].showObject = true;
	}
	break;
	case SC_PACKET_DEFFEND:
	{
		sc_packet_deffend* my_packet = reinterpret_cast<sc_packet_deffend*>(ptr);
		int deffend_idx = object_id_to_idx(my_packet->deffend_id);

		_matrix temp;
		temp._11 = my_packet->r_x;
		temp._12 = my_packet->r_y;
		temp._13 = my_packet->r_z;
		temp._21 = my_packet->u_x;
		temp._22 = my_packet->u_y;
		temp._23 = my_packet->u_z;
		temp._31 = my_packet->l_x;
		temp._32 = my_packet->l_y;
		temp._33 = my_packet->l_z;
		temp._41 = my_packet->p_x;
		temp._43 = my_packet->p_z;
		Set_Server_Mat(my_packet->deffend_id, &temp);

		if (FAILED(CManagement::GetInstance()->Add_GameObjectToLayer(L"GameObject_Deffend", (_uint)SCENEID::SCENE_STAGE, L"Layer_Deffend", nullptr, (void*)&temp, deffend_idx)))
			return;
		m_objects[my_packet->deffend_id].showObject = true;
	}
	break;
	case SC_PACKET_ANIMATION:
	{
		sc_packet_animation* my_packet = reinterpret_cast<sc_packet_animation*>(ptr);
		update_anim(my_packet->id, my_packet->anim);
	}
	break;
	case SC_PACKET_DEAD:
	{
		sc_packet_dead* my_packet = reinterpret_cast<sc_packet_dead*>(ptr);
		int recv_id = my_packet->id;
		m_objects[recv_id].hp = my_packet->hp;
		update_anim(recv_id, my_packet->anim);

		/*CGameObject* pGameObject = managment->Get_GameObject((_uint)SCENEID::SCENE_STAGE, L"Layer_Player", my_id);
		pGameObject->
		m_IsShow = pGameObject->GetIsShow();*/


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
	case SC_PACKET_TIMEDELTA:
	{
		sc_packet_timedelta* my_packet = reinterpret_cast<sc_packet_timedelta*>(ptr);
		time_delta = my_packet->time;
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

void CServer_Manager::send_move_packet(unsigned char dir)
{
	cs_packet_move m_packet;
	m_packet.type = CS_PACKET_MOVE;
	m_packet.size = sizeof(m_packet);
	m_packet.dir = dir;
	cout << "send move\n";
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

void CServer_Manager::send_time_end_packet()
{
	cs_packet_time_end m_packet;
	m_packet.type = CS_PACKET_TIME_END;
	m_packet.size = sizeof(m_packet);
	send_packet(&m_packet);
}

void CServer_Manager::send_end_pos_packet()
{
	cs_packet_end_pos m_packet;
	m_packet.type = CS_PACKET_END_POS;
	m_packet.size = sizeof(m_packet);
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

void CServer_Manager::send_deffend_packet()
{
	cs_packet_arrow l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = CS_PACKET_DEFFEND;
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
	if (A_HIT == anim || A_ATTACK == anim || A_DEAD == anim)
	{
		m_objects[id].anim_stat = anim;
		m_objects[id].isOnce = true;
	}
	else
		m_objects[id].isOnce = false;

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

void CServer_Manager::Set_Server_Mat(int id, _matrix* mat)
{
	m_objects[id].m_mat._11 = mat->_11;
	m_objects[id].m_mat._12 = mat->_12;
	m_objects[id].m_mat._13 = mat->_13;
	m_objects[id].m_mat._21 = mat->_21;
	m_objects[id].m_mat._22 = mat->_22;
	m_objects[id].m_mat._23 = mat->_23;
	m_objects[id].m_mat._31 = mat->_31;
	m_objects[id].m_mat._32 = mat->_32;
	m_objects[id].m_mat._33 = mat->_33;
	m_objects[id].m_mat._41 = mat->_41;
	m_objects[id].m_mat._42 = mat->_42;
	m_objects[id].m_mat._43 = mat->_43;
}

short CServer_Manager::object_idx_to_id(unsigned short id)
{
	return id + OBJECT_START;
}

short CServer_Manager::rock_idx_to_id(unsigned short id)
{
	return id + ROCK_START;
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

bool CServer_Manager::Get_Show(int index, char type)
{
	if (type == O_PLAYER)
		return m_objects[index].showObject;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(index)].showObject;
	else if (type == O_OBJECT)
		return m_objects[object_idx_to_id(index)].showObject;
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

short CServer_Manager::Get_HP(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].hp;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].hp;
	else if (type == O_OBJECT)
		return m_objects[object_idx_to_id(id)].hp;
}

bool CServer_Manager::Get_isFirst(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].isFirst;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].isFirst;
	else if (type == O_OBJECT)
		return m_objects[object_idx_to_id(id)].isFirst;
}

bool CServer_Manager::Get_isOnce(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].isOnce;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].isOnce;
	else if (type == O_OBJECT)
		return m_objects[object_idx_to_id(id)].isOnce;
}

void CServer_Manager::Set_isFirst(bool first, int id, char type)
{
	if (type == O_PLAYER)
	{
		m_objects[id].isFirst = first;
		m_objects[id].m_mat._42 = 0.f;
	}
	else if (type == O_NPC)
	{
		m_objects[npc_idx_to_id(id)].isFirst = first;
		m_objects[npc_idx_to_id(id)].m_mat._42 = 0.f;
	}
	else if (type == O_OBJECT)
	{
		m_objects[object_idx_to_id(id)].isFirst = first;
		m_objects[object_idx_to_id(id)].m_mat._42 = 0.f;
	}
}

void CServer_Manager::Set_isOnce(bool once, int id, char type)
{
	if (type == O_PLAYER)
		m_objects[id].isOnce = once;
	else if (type == O_NPC)
		m_objects[npc_idx_to_id(id)].isOnce = once;
	else if (type == O_OBJECT)
		m_objects[object_idx_to_id(id)].isOnce = once;
}

int CServer_Manager::Get_PlayerClass(int id)
{
	return m_objects[id].m_class;
}

_matrix CServer_Manager::Get_ServerMat(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].m_mat;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].m_mat;
	else if (type == O_OBJECT)
		return m_objects[object_idx_to_id(id)].m_mat;
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

bool CServer_Manager::Get_Particle(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].isParticle;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].isParticle;
}

void CServer_Manager::Set_Particle(int id, bool stat, char type)
{
	if (type == O_PLAYER)
		m_objects[id].isParticle = stat;
	else if (type == O_NPC)
		m_objects[npc_idx_to_id(id)].isParticle = stat;
}

float CServer_Manager::Get_TimeDelta()
{
	return time_delta;
}

short CServer_Manager::Get_ShowOtherPlayer(int id)
{
	return m_objects[id].showObject;
}

short CServer_Manager::Get_Anim(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].anim;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].anim;
}

short CServer_Manager::Get_AnimStat(int id, char type)
{
	if (type == O_PLAYER)
		return m_objects[id].anim_stat;
	else if (type == O_NPC)
		return m_objects[npc_idx_to_id(id)].anim_stat;
}

void CServer_Manager::Set_Anim(short anim, int id, char type)
{
	if (type == O_PLAYER)
		m_objects[id].anim = anim;
	else if (type == O_NPC)
		m_objects[npc_idx_to_id(id)].anim = anim;
}

bool CServer_Manager::Get_isInvisible(int id)
{
	return m_objects[id].isInvisible;
}

short CServer_Manager::Get_NpcSize()
{
	return my_npc;
}

float CServer_Manager::Get_Nature_Scale(int id)
{
	int index = object_idx_to_id(id);
	return m_objects[index].scale;
}

float CServer_Manager::Get_Rock_Scale(int id)
{
	int index = rock_idx_to_id(id);
	return m_objects[index].scale;
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

high_resolution_clock::time_point CServer_Manager::Get_Move_Cooltime()
{
	return move_ct;
}

high_resolution_clock::time_point CServer_Manager::Get_Rotate_Cooltime()
{
	return rotate_ct;
}

void CServer_Manager::Set_Move_CoolTime(high_resolution_clock::time_point ct)
{
	move_ct = ct;
}

void CServer_Manager::Set_Rotate_CoolTime(high_resolution_clock::time_point ct)
{
	rotate_ct = ct;
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
