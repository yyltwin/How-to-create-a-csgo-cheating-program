#include "main.h"
#include <iosfwd>

/*
2020.06.22
csgo������������
*/

unsigned long g_pid = 0;
unsigned long g_client = 0;
unsigned long g_engine = 0;

unsigned long dwClientState = 0;
unsigned long dwLocalPlayer = 0;
unsigned long dwEntityList = 0;
unsigned long m_hViewModel = 0;
unsigned long m_iViewModelIndex = 0;
unsigned long m_flFallbackWear = 0;
unsigned long m_nFallbackPaintKit = 0;
unsigned long m_iItemIDHigh = 0;
unsigned long m_iEntityQuality = 0;
unsigned long m_iItemDefinitionIndex = 0;
unsigned long m_hActiveWeapon = 0;
unsigned long m_hMyWeapons = 0;
unsigned long m_nModelIndex = 0;

//��ȡ�ڴ�
template<class T>
T read_memory(unsigned long addr)
{
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_pid);
	if (proc == INVALID_HANDLE_VALUE) return 0;

	SIZE_T bytes = 0;
	T temp;
	ReadProcessMemory(proc, (LPCVOID)addr, &temp, sizeof(T), &bytes);
	CloseHandle(proc);
	return temp;
}

//д���ڴ�
template<class T>
void write_memory(unsigned long addr, T temp)
{
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_pid);
	if (proc == INVALID_HANDLE_VALUE) return;

	SIZE_T bytes = 0;
	WriteProcessMemory(proc, (LPVOID)addr, (LPCVOID)&temp, sizeof(T), &bytes);
	CloseHandle(proc);
}

int main(int argc, char* arvg[])
{
	//��ȡ��Ϸ����ID
	unsigned long pid = get_process_id("Counter-Strike: Global Offensive");
	printf("[+] ����ID -> %d\n", pid);
	g_pid = pid;

	//��ȡ��Ϸ��Ҫģ���ַ
	g_client = get_module_addr("client.dll", pid);
	g_engine = get_module_addr("engine.dll", pid);
	printf("[+] client.dll -> 0x%8x\n", g_client);
	printf("[+] engine.dll -> 0x%8x\n", g_engine);

	//�������ϻ�ȡһЩ��ַ��ƫ��
	get_csgo_address_from_web();

	//��ʼcsgo����
	change_skin();

	return 0;
}

unsigned long get_process_id(const char* proc)
{
	unsigned long pid = 0;
	GetWindowThreadProcessId(FindWindowA(nullptr, proc), &pid);
	if (pid == 0) error_handle("δ����csgo��Ϸ����");
	return pid;
}

unsigned long get_module_addr(const char* mod, unsigned long pid)
{
	void* snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (snap == nullptr) error_handle("CreateToolhelp32Snapshotʧ��");

	MODULEENTRY32 module{ sizeof(MODULEENTRY32) };
	Module32First(snap, &module);

	do
	{
		if (strncmp(mod, module.szModule, sizeof(mod)) == 0)
			return (unsigned long)module.modBaseAddr;
	} while (Module32Next(snap, &module));

	error_handle(mod);
	return 0;
}

std::string get_web_code(const char* web)
{
	//������������
	const auto internet = InternetOpen("browser", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
	if (internet == nullptr) error_handle("InternetOpenʧ��");

	//���Է���ָ����ַ
	const auto internet_address = InternetOpenUrl(internet, web, nullptr, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
	if (internet_address == nullptr) error_handle("InternetOpenUrlʧ��");

	//��ȡ��ַȫ��Դ��
	std::string buffer;
	unsigned long read_bytes = 0;
	do
	{
		const int const_size = 1024 * 8;
		char temp[const_size]{ 0 };
		InternetReadFile(internet_address, reinterpret_cast<LPVOID>(temp), const_size, &read_bytes);
		buffer += temp;
	} while (read_bytes);

	InternetCloseHandle(internet_address);
	InternetCloseHandle(internet);
	return buffer;
}

void get_csgo_address_from_web()
{
	//�������ο���ַ
	//https://github.com/frk1/hazedumper/blob/master/csgo.cs
	//https://github.com/MrLotsmon/csoffsets/blob/master/csgo.cs
	std::string buffer = get_web_code("https://github.com/MrLotsmon/csoffsets/blob/master/csgo.cs");

	//Ӳ����ľ���û�еģ���ȷ���Ժ���Ϸ����ƫ��ʧЧ�Ŀ���

	dwClientState = get_address(buffer, "dwClientState");
	dwLocalPlayer = get_address(buffer, "dwLocalPlayer");
	dwEntityList = get_address(buffer, "dwEntityList");
	//m_hViewModel = get_address(buffer, "m_hViewModel");
	m_hViewModel = 0x32f8;
	//m_iViewModelIndex = get_address(buffer, "m_iViewModelIndex");
	m_iViewModelIndex = 0x3240;
	m_flFallbackWear = get_address(buffer, "m_flFallbackWear");
	m_nFallbackPaintKit = get_address(buffer, "m_nFallbackPaintKit");
	m_iItemIDHigh = get_address(buffer, "m_iItemIDHigh");
	m_iEntityQuality = get_address(buffer, "m_iEntityQuality");
	m_iItemDefinitionIndex = get_address(buffer, "m_iItemDefinitionIndex");
	m_hActiveWeapon = get_address(buffer, "m_hActiveWeapon");
	m_hMyWeapons = get_address(buffer, "m_hMyWeapons");
	//m_nModelIndex = get_address(buffer, "m_nModelIndex");
	m_nModelIndex = 0x258;
}

unsigned long get_address(const std::string& buffer, const char* str, int offset /*= 0*/)
{
	auto pos = buffer.find(str, offset);
	auto begin = buffer.find("0x", pos);
	auto end = buffer.find("<", begin);
	if (pos == std::string::npos || begin == std::string::npos || end == std::string::npos) error_handle(str);

	int addr = 0;
	std::stringstream translate;
	translate << buffer.substr(begin + 2, end - (begin + 2));
	translate >> std::hex >> addr;
	if (addr == 0) error_handle(str);

	printf("[+] %s -> %x\n", str, addr);
	return addr;
}

void change_skin()
{
	/*
	С��ģ�� -- С��ģ������
500  --  474
505  --  480
506  --  483
507  --  486
508  --  489
509  --  492
512  --  495
514  --  498
515  --  501
516  --  504
519  --  514
520  --  517
522  --  523
523  --  526
503  --  477
517  --  508
518  --  511
521  --  520
525  --  529
	*/

	//С��ģ��
	short knife_index = 500;

	//С��ģ������
	unsigned int knife_model_index = 474;

	const int item_id_high = -1;
	const int entity_quality = 3;
	const float fallback_wear = 0.001f;//ĥ��

	//�����޸�.....
	while (true)
	{
		//��ȡ����Լ���ַ
		unsigned long local_player = read_memory<unsigned long>(g_client + dwLocalPlayer);
		if (local_player == 0) continue;

		//��ȡ�������ϵ�ȫ��׼��
		for (int i = 0; i < 8; i++)
		{
			//��ȡ��ǰ����
			unsigned long current_weapon = read_memory<unsigned long>(local_player + m_hMyWeapons + i * 0x4) & 0xfff;
			current_weapon = read_memory<unsigned long>(g_client + dwEntityList + (current_weapon - 1) * 0x10);
			if (current_weapon == 0) continue;

			//��ȡ��������
			short weapon_index = read_memory<short>(current_weapon + m_iItemDefinitionIndex);

			//�����С��ģ��
			if (weapon_n::IsIDKnife(weapon_index))
			{
				write_memory<short>(current_weapon + m_iItemDefinitionIndex, knife_index);
				write_memory<unsigned int>(current_weapon + m_nModelIndex, knife_model_index);
				write_memory<unsigned int>(current_weapon + m_iViewModelIndex, knife_model_index);
				write_memory<int>(current_weapon + m_iEntityQuality, entity_quality);
			}

			//��ȡǹе/С�� Ƥ��
			unsigned int weapon_skin = weapon_n::get_waepon_skin(weapon_index);
			if (weapon_skin)
			{
				write_memory<int>(current_weapon + m_iItemIDHigh, item_id_high);
				write_memory<unsigned int>(current_weapon + m_nFallbackPaintKit, weapon_skin);
				write_memory<float>(current_weapon + m_flFallbackWear, fallback_wear);
			}
		}

		//��ȡ��ǰ������
		unsigned long active_weapon = read_memory<unsigned long>(local_player + m_hActiveWeapon) & 0xfff;
		active_weapon = read_memory<unsigned long>(g_client + dwEntityList + (active_weapon - 1) * 0x10);
		if (active_weapon == 0) continue;

		//��ǰ��������
		short index = read_memory<short>(active_weapon + m_iItemDefinitionIndex);
		//�õĲ���С���Ͳ���Ҫ����ִ����
		if (index != knife_index) continue;

		//��ȡ�����Լ����ӽ�(С��ģ����Ҫ�����Լ����ӽ�)
		unsigned long active_view_model = read_memory<unsigned long>(local_player + m_hViewModel) & 0xfff;
		active_view_model = read_memory<unsigned long>(g_client + dwEntityList + (active_view_model - 1) * 0x10);
		if (active_view_model == 0) continue;

		//д��С��ģ�͵����ǵ��ӽ�
		write_memory<unsigned int>(active_view_model + m_nModelIndex, knife_model_index);
	}
}