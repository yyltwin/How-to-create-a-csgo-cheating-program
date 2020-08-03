#pragma once

#include "address.hpp"
#include "memory.hpp"
#include "bsp_parser.hpp"

enum CSWeaponID_t : uint16_t //enum  ItemSchemaIndex_e
{
	WEAPON_NONE = 0,

	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER
};

class cheats
{
private:
	address m_address;			//��ַ������
	memory m_memory;		//�ڴ������
	rn::bsp_parser m_bsp;		//��ͼ������

	std::string m_path;			//����CSGO��·��

	DWORD m_client;			//��ַ
	DWORD m_engine;
	DWORD m_vstdlib;

	DWORD dwLocalPlayer;
	DWORD m_iTeamNum;
	DWORD dwGlowObjectManager;
	DWORD dwEntityList;
	DWORD m_bGunGameImmunity;
	DWORD m_iGlowIndex;
	DWORD dwClientState;
	DWORD dwClientState_Map;
	DWORD dwClientState_MapDirectory;
	DWORD dwGameDir;
	DWORD m_vecOrigin;
	DWORD m_vecViewOffset;
	DWORD m_dwBoneMatrix;
	DWORD m_aimPunchAngle;
	DWORD dwClientState_ViewAngles;
	DWORD m_iHealth;
	DWORD dwForceAttack;
	DWORD m_iItemDefinitionIndex;
	DWORD dwForceAttack2;
	DWORD m_bIsScoped;
	DWORD m_hActiveWeapon;
	DWORD dwForceBackward;
	DWORD dwForceForward;
	DWORD dwForceJump;
	DWORD dwForceLeft;
	DWORD dwForceRight;
	DWORD m_fFlags;
	DWORD m_lifeState;
	DWORD interface_engine_cvar;
	DWORD convar_name_hash_table;
	DWORD m_iFOVStart;

private:
	/* ��ȡ����λ�� */
	D3DXVECTOR3 get_bone_pos(DWORD base, int index)
	{
		typedef struct {
			float Matrix[3][4];
		} Matrix3x4_t;

		DWORD addr = m_memory.read<DWORD>(base + m_dwBoneMatrix);
		if (addr)
		{
			Matrix3x4_t result = m_memory.read<Matrix3x4_t>(addr + index * 0x30);

			return{
				result.Matrix[0][3],
				result.Matrix[1][3],
				result.Matrix[2][3]
			};
		}

		return {  };
	}

	/* ��ȡ��Ӫ */
	int get_team(DWORD base)
	{
		return m_memory.read<int>(base + m_iTeamNum);
	}

	/* ��ȡ��ǹ״̬ */
	bool get_immunity(DWORD base)
	{
		return m_memory.read<bool>(base + m_bGunGameImmunity);
	}

	/* ��ȡ����״̬ */
	bool get_scoped(DWORD base)
	{
		return m_memory.read<bool>(base + m_bIsScoped);
	}

	/* ��ȡ����״̬ */
	bool get_two_scoped(DWORD base)
	{
		return 90 != m_memory.read<int>(base + m_iFOVStart);
	}

	/* ��ȡ���״̬ */
	bool get_life_state(DWORD base)
	{
		int health = m_memory.read<int>(base + m_iHealth);
		unsigned char state = m_memory.read<unsigned char>(base + m_lifeState);
		return health && !state;
	}

	/* ��ȡ����Ƿ����� */
	bool get_player_life(int index)
	{
		DWORD entity = m_memory.read<DWORD>(m_client + dwEntityList + index * 0x10);
		if (entity == false) return false;

		return get_life_state(entity);
	}

	/* �жϵ�ǰ�����Ƿ��ܿ��� */
	bool IsIDSniper(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_AWP:
			return true;
		case WEAPON_G3SG1:
			return true;
		case WEAPON_SCAR20:
			return true;
		case WEAPON_SSG08:
			return true;
		default:
			return false;
		}
	}

	/* �жϵ�ǰ�����Ƿ���С�� */
	bool IsIDKnife(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_KNIFE:
			return true;
		case WEAPON_KNIFEGG:
			return true;
		case WEAPON_KNIFE_T:
			return true;
		case WEAPON_KNIFE_GUT:
			return true;
		case WEAPON_KNIFE_FLIP:
			return true;
		case WEAPON_BAYONET:
			return true;
		case WEAPON_KNIFE_M9_BAYONET:
			return true;
		case WEAPON_KNIFE_KARAMBIT:
			return true;
		case WEAPON_KNIFE_TACTICAL:
			return true;
		case WEAPON_KNIFE_BUTTERFLY:
			return true;
		case WEAPON_KNIFE_FALCHION:
			return true;
		case WEAPON_KNIFE_PUSH:
			return true;
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
			return true;
		default:
			return false;
		}
	}

	/* �жϵ�ǰ�����Ƿ������� */
	bool IsIDOther(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_NONE:
			return true;
		case WEAPON_TASER:
			return true;
		case WEAPON_FLASHBANG:
			return true;
		case WEAPON_HEGRENADE:
			return true;
		case WEAPON_SMOKEGRENADE:
			return true;
		case WEAPON_MOLOTOV:
			return true;
		case WEAPON_DECOY:
			return true;
		case WEAPON_INCGRENADE:
			return true;
		case WEAPON_C4:
			return true;
		default:
			return false;
		}
	}

	/* ��ȡ��ǰ�������� */
	short get_weapon_index(DWORD base)
	{
		int index = m_memory.read<DWORD>(base + m_hActiveWeapon) & 0xfff;
		base = m_memory.read<DWORD>(m_client + dwEntityList + (index - 1) * 0x10);
		return m_memory.read<short>(base + m_iItemDefinitionIndex);
	}

	/* ��������Ƕ� */
	D3DXVECTOR3 get_aimbot_angle(D3DXVECTOR3& vecOrigin, D3DXVECTOR3& vecOther)
	{
		D3DXVECTOR3 vecAngles{};
		D3DXVECTOR3 vecDelta = D3DXVECTOR3((vecOrigin[0] - vecOther[0]), (vecOrigin[1] - vecOther[1]), (vecOrigin[2] - vecOther[2]));
		float hyp = sqrtf(vecDelta[0] * vecDelta[0] + vecDelta[1] * vecDelta[1]);

		float  M_PI = 3.14159265358979323846f;
		vecAngles[0] = (float)atan(vecDelta[2] / hyp)		*(float)(180.f / M_PI);
		vecAngles[1] = (float)atan(vecDelta[1] / vecDelta[0])	*(float)(180.f / M_PI);
		vecAngles[2] = (float)0.f;

		if (vecDelta[0] >= 0.f) vecAngles[1] += 180.0f;
		return vecAngles;
	}

	/* �Ƕȹ�һ�� */
	void angle_normalize(D3DXVECTOR3& vAngles)
	{
		for (int i = 0; i < 3; i++)
		{
			if (vAngles[i] < -180.0f) vAngles[i] += 360.0f;
			if (vAngles[i] > 180.0f) vAngles[i] -= 360.0f;
		}

		if (vAngles.x < -89.0f) vAngles.x = 89.0f;
		if (vAngles.x > 89.0f) vAngles.x = 89.0f;

		vAngles.z = 0;
	}

	/*  */
	void clamp_angles(D3DXVECTOR3& vAngles)
	{
		while (vAngles.y < -180.0f) vAngles.y += 360.0f;
		while (vAngles.y > 180.0f) vAngles.y -= 360.0f;

		if (vAngles.x < -89.0f) vAngles.x = 89.0f;
		if (vAngles.x > 89.0f) vAngles.x = 89.0f;

		vAngles.z = 0;
	}

	/* ��ȡ�ַ���hash */
	int get_string_hash(const char* name)
	{
		struct help { int code[256]{ 0 }; };
		help buf = m_memory.read<help>(m_vstdlib + convar_name_hash_table);

		int v2 = 0, v3 = 0, size = strlen(name);
		for (int i = 0; i < size; i += 2)
		{
			v3 = buf.code[v2 ^ toupper(name[i])];
			if (i + 1 == size) break;
			v2 = buf.code[v3 ^ toupper(name[i + 1])];
		}
		return v2 | (v3 << 8);
	}

	/* ��ȡ���ĵ�ַ */
	DWORD get_convar_address(const char* name)
	{
		int hash = get_string_hash(name);
		DWORD convar_engine = m_memory.read<DWORD>(m_vstdlib + interface_engine_cvar);
		if (convar_engine == 0) return 0;

		DWORD addr = m_memory.read<DWORD>(convar_engine + 0x34);
		if (addr == 0) return 0;

		DWORD point = m_memory.read<DWORD>(addr + ((unsigned char)hash * 4));
		while (point)
		{
			if (m_memory.read<DWORD>(point) == hash)
			{
				DWORD convar_point = m_memory.read<DWORD>(point + 0x4);
				if (convar_point)
				{
					struct help { char value[32]{ 0 }; };
					DWORD temp = m_memory.read<DWORD>(convar_point + 0xC);
					if (temp)
					{
						help buf = m_memory.read<help>(temp);
						if (strcmp(buf.value, name) == 0)
							return convar_point;
					}
				}
			}

			point = m_memory.read<DWORD>(point + 0xC);
		}

		return 0;
	}

	/* ���������ֵ */
	void set_convar_value(const char* name, int value)
	{
		DWORD addr = get_convar_address(name);
		if (addr) m_memory.write<int>(addr + 0x30, value ^ addr);
	}

	/* ���������� */
	inline void mouse_left_down()
	{
		m_memory.write<bool>(m_client + dwForceAttack, true);
		Sleep(5);
		m_memory.write<bool>(m_client + dwForceAttack, false);
	}

public:
	/* ��ȡ�Ƕ� */
	D3DXVECTOR3 get_angle()
	{
		DWORD addr = m_memory.read<DWORD>(m_engine + dwClientState);
		if (addr)
		{
			return m_memory.read<D3DXVECTOR3>(addr + dwClientState_ViewAngles);
		}
		return {};
	}

	/* ���ýǶ� */
	void set_angle(D3DXVECTOR3& angle)
	{
		DWORD addr = m_memory.read<DWORD>(m_engine + dwClientState);
		if (addr)
		{
			m_memory.write<D3DXVECTOR3>(addr + dwClientState_ViewAngles, angle);
		}
	}

public:
	cheats() {}
	~cheats() {}

	/* ��ʼ�� */
	bool initialize()
	{
		/* ���Ի�ȡCSGO����ID */
		if (m_memory.attach("csgo.exe") == false) return false;
		std::cout << "[+] ��ǰ��ϷID : " << m_memory.get_pid() << std::endl;

		/* ���Ի�ȡ��ַ */
		MODULEENTRY32 client = m_memory.find_module("client.dll");
		MODULEENTRY32 engine = m_memory.find_module("engine.dll");
		MODULEENTRY32 vstdlib = m_memory.find_module("vstdlib.dll");
		if (client.modBaseSize == 0 || engine.modBaseSize == 0 || vstdlib.modBaseSize == 0) return false;
		m_client = (DWORD)client.modBaseAddr;
		m_engine = (DWORD)engine.modBaseAddr;
		m_vstdlib = (DWORD)vstdlib.modBaseAddr;
		std::cout << "[+] client��ַ : 0x" << std::hex << m_client << std::endl;
		std::cout << "[+] engine��ַ : 0x" << std::hex << m_engine << std::endl;
		std::cout << "[+] vstdlib��ַ : 0x" << std::hex << m_vstdlib << std::endl;

		/* ���Ի�ȡ��ַ��ƫ�� */
		if (m_address.Initialize() == false) return false;
		dwLocalPlayer = m_address.get_address("dwLocalPlayer");
		m_iTeamNum = m_address.get_address("m_iTeamNum");
		dwGlowObjectManager = m_address.get_address("dwGlowObjectManager");
		dwEntityList = m_address.get_address("dwEntityList");
		m_bGunGameImmunity = m_address.get_address("m_bGunGameImmunity");
		m_iGlowIndex = m_address.get_address("m_iGlowIndex");
		dwClientState = m_address.get_address("dwClientState");
		dwClientState_Map = m_address.get_address("dwClientState_Map");
		dwClientState_MapDirectory = m_address.get_address("dwClientState_MapDirectory");
		dwGameDir = m_address.get_address("dwGameDir");
		m_vecOrigin = m_address.get_address("m_vecOrigin");
		m_vecViewOffset = m_address.get_address("m_vecViewOffset");
		m_dwBoneMatrix = m_address.get_address("m_dwBoneMatrix");
		m_aimPunchAngle = m_address.get_address("m_aimPunchAngle");
		dwClientState_ViewAngles = m_address.get_address("dwClientState_ViewAngles");
		m_iHealth = m_address.get_address("m_iHealth");
		dwForceAttack = m_address.get_address("dwForceAttack");
		m_iItemDefinitionIndex = m_address.get_address("m_iItemDefinitionIndex");
		dwForceAttack2 = m_address.get_address("dwForceAttack2");
		m_bIsScoped = m_address.get_address("m_bIsScoped");
		m_hActiveWeapon = m_address.get_address("m_hActiveWeapon");
		dwForceBackward = m_address.get_address("dwForceBackward");
		dwForceForward = m_address.get_address("dwForceForward");
		dwForceJump = m_address.get_address("dwForceJump");
		dwForceLeft = m_address.get_address("dwForceLeft");
		dwForceRight = m_address.get_address("dwForceRight");
		m_fFlags = m_address.get_address("m_fFlags");
		m_lifeState = m_address.get_address("m_lifeState");
		interface_engine_cvar = m_address.get_address("interface_engine_cvar");
		convar_name_hash_table = m_address.get_address("convar_name_hash_table");
		m_iFOVStart = m_address.get_address("m_iFOVStart");

		std::string temp = m_memory.read_string(m_engine + dwGameDir, 150);
		m_path = temp.substr(0, strlen(temp.c_str())) + "\\maps";
		std::cout << "[+] ��ǰ��Ϸ·�� : " << m_path << std::endl;

		return true;
	}

	/* ���µ�ͼ */
	void update_map()
	{
		static std::string last_map = "";
		static bool state = false;

		DWORD addr = m_memory.read<DWORD>(m_engine + dwClientState);
		if (addr)
		{
			std::string temp = m_memory.read_string(addr + dwClientState_Map, 64);
			std::string current_map = temp.substr(0, strlen(temp.c_str())) + ".bsp";
			if (state == false || last_map != current_map)
			{
				last_map = current_map;
				state = m_bsp.load_map(m_path, current_map);
				std::cout << "[+] ��ǰ��Ϸ��ͼ : " << last_map << " - " << (state == true ? "�ɹ�" : "ʧ��") << std::endl;
				Sleep(300);
			}
		}
	}

	/* ����Թ� */
	void glow()
	{
		/* ��ȡ�Լ���ַ */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return;

		/* ��ȡ�Լ����״̬ */
		if (get_life_state(local_player) == false) return;

		/* ��ȡ�Լ���Ӫ */
		int local_team = get_team(local_player);

		/* ��ȡ�Լ��۾�λ�� */
		D3DXVECTOR3 local_pos = m_memory.read<D3DXVECTOR3>(local_player + m_vecOrigin);
		local_pos += m_memory.read<D3DXVECTOR3>(local_player + m_vecViewOffset);

		/* ��ȡ�Թ������ */
		DWORD glow_manager = m_memory.read<DWORD>(m_client + dwGlowObjectManager);
		if (glow_manager == 0) return;

		/* ������� */
		for (int i = 0; i < 64; i++)
		{
			/* ��ȡ��һ�ַ */
			DWORD entity = m_memory.read<DWORD>(m_client + dwEntityList + i * 0x10);
			if (entity == 0 || entity == local_player) continue;

			/* ��ȡ���Ѫ�� */
			if (get_life_state(entity) == false) continue;

			/* ��ȡ�����Ӫ */
			int entity_team = get_team(entity);
			if (entity_team == local_team) continue;

			/* ��ȡ�����ǹ״̬ */
			if (get_immunity(entity)) continue;

			/* ��ȡ��һԹ����� */
			int entity_glow = m_memory.read<int>(entity + m_iGlowIndex);

			/* ��ȡ���ͷ������ */
			D3DXVECTOR3 bone = get_bone_pos(entity, 8);

			/* ��ɫ */
			float color = 2.0f;

			/* �ж���ҿɼ�״̬ */
			if (m_bsp.is_visible(local_pos, bone)) color = 0.0f;

			/* ʵ�ֻԹ� */
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0x4), 2.0f);
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0x8), color);
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0xc), 0.0f);
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0x10), 0.5f);

			m_memory.write<bool>(glow_manager + ((entity_glow * 0x38) + 0x24), true);
			m_memory.write<bool>(glow_manager + ((entity_glow * 0x38) + 0x25), false);
		}
	}

	/* �������� */
	bool  aimbot()
	{
		/* ��ȡ�Լ���ַ */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return false;

		/* ��ȡ�Լ����״̬ */
		if (get_life_state(local_player) == false) return false;

		/* ����һ���ӽ� */
		m_memory.write<DWORD>(local_player + 0x332c, 120);

		/* ��ȡ��ǰ�������� */
		short weapon_index = get_weapon_index(local_player);
		if (IsIDKnife(weapon_index) || IsIDOther(weapon_index)) return false;

		/* ��ȡ�Լ��۾�λ�� */
		D3DXVECTOR3 local_pos = m_memory.read<D3DXVECTOR3>(local_player + m_vecOrigin);
		local_pos += m_memory.read<D3DXVECTOR3>(local_player + m_vecViewOffset);

		/* ��ȡ�Լ���Ӫ */
		int local_team = get_team(local_player);

		/* ������� */
		for (int i = 0; i < 64; i++)
		{
			/* ��ȡ��һ�ַ */
			DWORD entity = m_memory.read<DWORD>(m_client + dwEntityList + i * 0x10);
			if (entity == 0 || entity == local_player) continue;

			/* �ж�Ѫ�� */
			if (get_life_state(entity) == false) continue;

			/* ��ȡ�����Ӫ */
			if (local_team == get_team(entity)) continue;

			/* ��ȡ�����ǹ״̬ */
			if (get_immunity(entity)) continue;

			/* ��ȡ���ͷ������ */
			D3DXVECTOR3 bone = get_bone_pos(entity, 8);

			/* �ж��Ƿ�ɼ� */
			if (m_bsp.is_visible(local_pos, bone) == false) continue;

			/* ��������Ƕ� */
			D3DXVECTOR3 angle = get_aimbot_angle(local_pos, bone);

			/* ��ȡ������ */
			D3DXVECTOR3 punch = m_memory.read<D3DXVECTOR3>(local_player + m_aimPunchAngle) * 2;
			if (abs(punch.x) > 1.0f) continue;
			angle -= punch;

			/* ��һ���Ƕ� */
			angle_normalize(angle);
			//clamp_angles(angle);

			/* ���ýǶ� */
			set_angle(angle);

			/* �Ƿ��ô��� */
			bool top = FindWindowA(nullptr, "Counter-Strike: Global Offensive") == GetForegroundWindow();

			/* ���� */
			if (top && IsIDSniper(weapon_index))
			{
				if (get_scoped(local_player) == false)
				{
					mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					Sleep(150);//С��
					//Sleep(400);//���
					return false;
				}
				//else if (get_two_scoped(local_player) == false) mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}

			/* ��ǹ */
			mouse_left_down();
			return true;
		}

		return false;
	}

	/* ��Ĭ���� */
	bool silent()
	{
		static bool last_status = false;
		static D3DXVECTOR3 angle;
		if (last_status == false) angle = get_angle();
		else
		{
			set_angle(angle);
			last_status = false;
		}
		last_status = aimbot();
		return last_status;
	}

	/* �������� */
	void bunnyhop()
	{
#define FORCE_JUMP_BITMASK (1<<0)

		/* ��ȡ�Լ���ַ */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return;

		/* ��ȡ�Լ����״̬ */
		if (get_life_state(local_player) == false) return;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			int state = m_memory.read<int>(m_client + dwForceJump);
			unsigned char ground = m_memory.read<unsigned char>(local_player + m_fFlags);
			if (ground) state |= FORCE_JUMP_BITMASK;
			else state &= ~FORCE_JUMP_BITMASK;
			m_memory.write<int>(m_client + dwForceJump, state);
		}
	}

	/* ������ */
	void big_top()
	{
		/* ��ȡ�Լ���ַ */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return;

		/* ��ȡ�Լ����״̬ */
		if (get_life_state(local_player) == false) return;

		/* ��ȡ��ǰ�������� */
		short weapon_index = get_weapon_index(local_player);
		if (IsIDKnife(weapon_index) || IsIDOther(weapon_index)) return;

		/* ����ǰ�ĽǶ� */
		static D3DXVECTOR3 last_angle{ 0,0,0 };

		if (m_memory.read<bool>(m_client + dwForceForward) == false
			&& m_memory.read<bool>(m_client + dwForceBackward) == false
			&& m_memory.read<bool>(m_client + dwForceLeft) == false
			&& m_memory.read<bool>(m_client + dwForceRight) == false)
		{
			if (last_angle.x == 0.0f &&last_angle.y == 0.0f) last_angle = get_angle();

			D3DXVECTOR3 last_angle{ 89,0,0 };
			last_angle.x = rand() % 90;
			last_angle.y = rand() % 180;
			if (rand() % 2 == 0)
			{
				last_angle.x = -last_angle.x;
				last_angle.y = -last_angle.y;
			}
			set_angle(last_angle);
		}
		else
		{
			if (last_angle.x && last_angle.y)
			{
				set_angle(last_angle);
				last_angle = { 0,0,0 };
			}
		}
	}

	/* ���ֵ���� */
	void handle_convar()
	{
		/* ���ú��ڴ��� */
		set_convar_value(AY_OBFUSCATE("mat_postprocess_enable"), 0);

		/* û����� */
		set_convar_value(AY_OBFUSCATE("r_3dsky"), 0);

		/* �������� */
		set_convar_value(AY_OBFUSCATE("r_drawparticles"), 0); $$$;

		/* �Ƴ���Ӱ */
		set_convar_value(AY_OBFUSCATE("cl_csm_enabled"), 0);

		/* �Ӿ������� */
		set_convar_value(AY_OBFUSCATE("view_recoil_tracking"), 1); $$$;

		/* ���������� */
		set_convar_value(AY_OBFUSCATE("cl_crosshair_recoil"), 1); $$$;
		set_convar_value(AY_OBFUSCATE("weapon_debug_spread_show"), 3); $$$;

		/* ��ʾͶ�׹켣 */
		set_convar_value(AY_OBFUSCATE("cl_grenadepreview"), 1); $$$;

		set_convar_value(AY_OBFUSCATE("cl_forwardspeed"), 450); $$$;
		set_convar_value(AY_OBFUSCATE("cl_sidespeed"), 450); $$$;
		set_convar_value(AY_OBFUSCATE("cl_backspeed"), 450); $$$;
	}
};