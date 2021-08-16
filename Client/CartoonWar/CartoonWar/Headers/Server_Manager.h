#pragma once
#include "Base.h"
class CManagement;
class CTransform;
class CServer_Manager :
    public CBase
{
    _DECLARE_SINGLETON(CServer_Manager);
private:
    CServer_Manager();
    virtual ~CServer_Manager() = default;

private:
    CManagement* managment;
    unordered_map <int, Session> m_objects;
    Flag flags[5];
    short my_id;
    short my_npc;
    short my_hp;
    short my_troop;
    short my_last_troop;
    bool isSendOnePacket;
    bool isConnected = false;
    bool isRed;
    bool isBlue;
    string m_ip;

    WSAEVENT m_EventArray;

    SOCKET m_cSocket; // 서버와 연결할 소켓
    string m_client_IP; // 클라 IP
    short PORT = 3500;
    int game_time = 0;
    WPARAM m_wparam;
    high_resolution_clock::time_point add_npc_ct;
    high_resolution_clock::time_point attack_ct;
    high_resolution_clock::time_point change_formation_ct;

public:
    BOOL InitServer(HWND hWnd);

    void ProcessPacket(char* ptr);
    void process_data(char* net_buf, size_t io_byte);
    HRESULT EventManager();

    void send_packet(void* packet);
    void send_condition_packet(unsigned char con_type, unsigned char con);
    void send_login_ok_packet();
    void send_add_npc_packet();
    void send_npc_act_packet(unsigned char act);
    void send_change_formation_packet();
    void send_animation_packet(unsigned char anim);
    void send_attack_packet();
    void send_position_packet(_vec3* pos);
    void send_class_change_packet(int idx, char type);
    void send_change_troop();
    void send_fire_packet(float mx, float mz);
    void send_teleport_packet(float mx, float mz);
    void send_arrow_packet();
    void send_invisible_packet(bool isinvi);

    void update_key_input();
    void update_anim(int id, unsigned char anim);

    //short player_index(unsigned short id);
    short npc_idx_to_id(unsigned short id);
    short npc_id_to_idx(unsigned short id);
    short object_idx_to_id(unsigned short id);
    short object_id_to_idx(unsigned short id);

    bool Get_ShowPlayer();
    bool Get_ShowNPC(int npc_index);
    bool Get_Blue(int id);
    bool Get_Red(int id);
    bool Get_Connected();
    short Get_PlayerID();
    short Get_PlayerHP(int id);
    int Get_PlayerClass(int id);
    int Get_NpcClass(int id);

    char Get_PlayerMCon(int id);
    char Get_NpcMCon(int id);
    void Set_PlayerMCon(char cond);
    char Get_PlayerRCon(int id);
    char Get_NpcRCon(int id);

    short Get_NpcHP(int id);
    short Get_ShowOtherPlayer(int id);
    short Get_Anim(int id);
    short Get_AnimNPC(int id);
    float Get_GameTime();
    short Get_NpcSize();
    short Get_TroopClass();
    bool Get_isHitPL(int id);
    bool Get_isHitNPC(int id);
    bool Get_isInvisible(int id);

    void Set_TroopClass(short mclass);
    void Set_AnimPL(int id, short anim);
    void Set_AnimNPC(int id, short anim);
    void Set_isHitPL(int id, bool ishit);
    void Set_isHitNPC(int id, bool ishit);

    bool is_player(int id);
    bool is_npc(int id);
    bool is_object(int id);

    high_resolution_clock::time_point Get_AddNPC_Cooltime();
    high_resolution_clock::time_point Get_Attack_Cooltime();
    high_resolution_clock::time_point Get_ChangeFormation_Cooltime();
    WPARAM Get_wParam();

    void Set_PlayerRCon(char cond);
    void Set_Class(int mclass, int id, char type);
    void Set_AddNPC_CoolTime(high_resolution_clock::time_point ct);
    void Set_Attack_CoolTime(high_resolution_clock::time_point ct);
    void Set_ChangeFormation_CoolTime(high_resolution_clock::time_point ct);
    void Set_wParam(WPARAM p);
    void Set_Anim(unsigned short anim);
    //void Set_AnimNPC(unsigned short anim);

    void init_client();

    void err_quit(const char* msg);
    void disconnect();

private:
    virtual void Free();
};




//HRESULT CScene_Stage::Ready_Layer_Orc03(const _tchar* pLayerTag, CManagement* pManagement)
//{
//	for (int i = 0; i < 28; ++i)
//	{
//		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Orc03", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, nullptr, i)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//HRESULT CScene_Stage::Ready_Layer_Flag(const _tchar* pLayerTag, CManagement* pManagement)
//{
//	for (int i = 0; i < 5; ++i)
//	{
//		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Flag", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, nullptr, i)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//HRESULT CScene_Stage::Ready_Layer_Rect(const _tchar* pLayerTag, CManagement* pManagement)
//{
//	for (int i = 0; i < 5; ++i)
//	{
//		if (FAILED(pManagement->Add_GameObjectToLayer(L"GameObject_Rect", (_uint)SCENEID::SCENE_STAGE, pLayerTag, nullptr, nullptr, i)))
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//_int CUI_HP::Update_GameObject(const _float& fTimeDelta)
//{
//	CServer_Manager* server = CServer_Manager::GetInstance();
//	if (nullptr == server)
//		return -1;
//	server->AddRef();
//
//
//
//	if (ENUM_PLAYER1 == server->Get_PlayerID())
//	{
//		//if (FAILED(managment->Add_GameObjectToLayer(L"GameObject_Orc02", (_uint)SCENEID::SCENE_STAGE, L"Layer_Orc02")))
//		//	return;
//		m_tInfo = CManagement::GetInstance()->Get_Layer((_uint)SCENEID::SCENE_STAGE, L"Layer_Orc02")->Get_BackObject()->GetInfo();
//	}
//	else if (ENUM_PLAYER2 == server->Get_PlayerID())
//	{
//		//if (FAILED(managment->Add_GameObjectToLayer(L"GameObject_Orc04", (_uint)SCENEID::SCENE_STAGE, L"Layer_Orc04")))
//		//	return;
//		m_tInfo = CManagement::GetInstance()->Get_Layer((_uint)SCENEID::SCENE_STAGE, L"Layer_Orc04")->Get_BackObject()->GetInfo();
//	}
//	Safe_Release(server);
//	return _int();
//}
//
//
//void CMyRect::Render_GameObject()
//{
//	CManagement* pManagement = CManagement::GetInstance();
//	if (nullptr == pManagement)
//		return;
//	pManagement->AddRef();
//
//
//	MAINPASS tMainPass = {};
//	_matrix matWorld = m_pTransformCom->Get_Matrix();
//	_matrix matView = CCamera_Manager::GetInstance()->GetMatView();
//	_matrix matProj = CCamera_Manager::GetInstance()->GetMatProj();
//
//	m_pShaderCom[0]->SetUp_OnShader(matWorld, matView, matProj, tMainPass);
//	_uint iOffset = pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->SetData((void*)&tMainPass);
//	CDevice::GetInstance()->SetConstantBufferToShader(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b0)->GetCBV().Get(), iOffset, CONST_REGISTER::b0);
//
//
//
//	if (m_iNum == 0)
//		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default")->GetSRV().Get(), TEXTURE_REGISTER::t0);
//	else if (m_iNum == 1)
//		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_1")->GetSRV().Get(), TEXTURE_REGISTER::t0);
//	else if (m_iNum == 2)
//		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_2")->GetSRV().Get(), TEXTURE_REGISTER::t0);
//	else if (m_iNum == 3)
//		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_3")->GetSRV().Get(), TEXTURE_REGISTER::t0);
//	else if (m_iNum == 4)
//		CDevice::GetInstance()->SetTextureToShader(pManagement->Get_UAV(L"UAV_Default_4")->GetSRV().Get(), TEXTURE_REGISTER::t0);
//	CDevice::GetInstance()->UpdateTable();
//
//
//
//
//	CDevice::GetInstance()->ClearDummyDesc_CS();
//	{
//		CTransform* pTransform_Red = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
//			L"Layer_Orc02", L"Com_Transform", 0);
//
//		CTransform* pTransform_Blue = (CTransform*)CManagement::GetInstance()->Get_ComponentPointer((_uint)SCENEID::SCENE_STAGE,
//			L"Layer_Orc04", L"Com_Transform", 0);
//
//		_vec3 vThisPos = *m_pTransformCom->Get_StateInfo(CTransform::STATE_POSITION);
//		_vec3 vRedPos = *pTransform_Red->Get_StateInfo(CTransform::STATE_POSITION);
//		_vec3 vBluePos = *pTransform_Blue->Get_StateInfo(CTransform::STATE_POSITION);
//
//		_vec3 vLen_Red = vThisPos - vRedPos;
//		_vec3 vLen_Blue = vThisPos - vBluePos;
//
//		_uint iLen_Red = Vector3_::Length(vLen_Red);
//		_uint iLen_Blue = Vector3_::Length(vLen_Blue);
//
//		CServer_Manager* server = CServer_Manager::GetInstance();
//		if (nullptr == server)
//			return;
//		server->AddRef();
//
//		if (server->Get_Red(m_iLayerIdx) && !(server->Get_Blue(m_iLayerIdx)))
//		{
//			m_tRep.m_arrInt[0] = 1;
//			m_eCurTeam = TEAM::TEAM_RED;
//		}
//		if (!(server->Get_Red(m_iLayerIdx)) && server->Get_Blue(m_iLayerIdx))
//		{
//			m_tRep.m_arrInt[1] = 1;
//			m_eCurTeam = TEAM::TEAM_BLUE;
//		}
//		if (server->Get_Red(m_iLayerIdx) && server->Get_Blue(m_iLayerIdx))
//		{
//			m_tRep.m_arrInt[1] = 0;
//			m_eCurTeam = TEAM::TEAM_END;
//		}
//		if (!(server->Get_Red(m_iLayerIdx)) && !(server->Get_Blue(m_iLayerIdx)))
//		{
//			m_tRep.m_arrInt[0] = 0;
//			m_eCurTeam = TEAM::TEAM_END;
//		}
//
//
//		iOffset = CManagement::GetInstance()->GetConstantBuffer((_uint)CONST_REGISTER::b8)->SetData((void*)&m_tRep);
//		CDevice::GetInstance()->SetUpContantBufferToShader_CS(pManagement->GetConstantBuffer((_uint)CONST_REGISTER::b8)->GetCBV().Get(), iOffset, CONST_REGISTER::b8);
//		if (m_iNum == 0)
//			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default"), UAV_REGISTER::u0);
//		else if (m_iNum == 1)
//			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_1"), UAV_REGISTER::u0);
//		else if (m_iNum == 2)
//			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_2"), UAV_REGISTER::u0);
//		else if (m_iNum == 3)
//			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_3"), UAV_REGISTER::u0);
//		else if (m_iNum == 4)
//			CDevice::GetInstance()->SetUpUAVToRegister(pManagement->Get_UAV(L"UAV_Default_4"), UAV_REGISTER::u0);
//
//		if ((m_tRep.m_arrInt[0] ^ m_tRep.m_arrInt[1]))
//		{
//			if (!m_IsFix)
//			{
//				m_tRep.m_arrFloat[0] += m_fDeltaTime;
//				m_fTempTime += m_fDeltaTime * 100.f;
//				m_IsTemp = false;
//				m_eCurTeam;
//				if (m_eCurTeam != m_ePreTeam)
//				{
//					m_fDeltaTime = 0.f;
//					m_fTempTime = 0.f;
//					m_IsTemp = true;
//					m_tRep.m_arrFloat[0] = 0.f;
//					switch (m_eCurTeam)
//					{
//					case TEAM::TEAM_RED:
//						m_eCurTeam = TEAM::TEAM_RED;
//						break;
//					case TEAM::TEAM_BLUE:
//						m_eCurTeam = TEAM::TEAM_BLUE;
//						break;
//					default:
//						break;
//					}
//					m_ePreTeam = m_eCurTeam;
//
//				}
//
//				m_tRep.m_arrInt[3] = (_uint)m_IsTemp;
//				m_pShaderCom[1]->UpdateData_CS();
//
//				if (m_iNum == 0)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 1)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_1")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 2)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_2")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 3)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_3")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 4)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_4")->Dispatch(1, 1000, 1);
//			}
//
//		}
//		else
//		{
//			if (m_eCurTeam != m_ePreTeam)
//			{
//				m_fDeltaTime = 0.f;
//				m_fTempTime = 0.f;
//				m_IsTemp = true;
//				m_tRep.m_arrFloat[0] = 0.f;
//				switch (m_eCurTeam)
//				{
//				case TEAM::TEAM_RED:
//					m_eCurTeam = TEAM::TEAM_RED;
//					break;
//				case TEAM::TEAM_BLUE:
//					m_eCurTeam = TEAM::TEAM_BLUE;
//					break;
//				case TEAM::TEAM_END:
//					m_eCurTeam = TEAM::TEAM_END;
//					break;
//				default:
//					break;
//				}
//				m_ePreTeam = m_eCurTeam;
//			}
//
//			if (!m_IsFix)
//			{
//				m_tRep.m_arrFloat[0] = 0.f;
//				m_fDeltaTime = 0.f;
//				m_IsTemp = true;
//				m_tRep.m_arrInt[3] = (_uint)m_IsTemp;
//				m_pShaderCom[1]->UpdateData_CS();
//				if (m_iNum == 0)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 1)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_1")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 2)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_2")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 3)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_3")->Dispatch(1, 1000, 1);
//				else if (m_iNum == 4)
//					CManagement::GetInstance()->Get_UAV(L"UAV_Default_4")->Dispatch(1, 1000, 1);
//			}
//
//		}
//		Safe_Release(server);
//	}
//
//
//	m_pBufferCom->Render_VIBuffer();
//	Safe_Release(pManagement);
//}