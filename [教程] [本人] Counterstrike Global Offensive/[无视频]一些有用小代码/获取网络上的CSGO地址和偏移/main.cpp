/*
CSGO��ַ��ƫ�ƻ�ȡ���
*/

#include <Windows.h>

#include <WinInet.h>
#pragma comment(lib,"Wininet.lib")

#include <iostream>
#include <string>
#include <sstream>

class address
{
private:
	std::string m_data;

public:
	address() {}
	~address() { }

	/* ��ʼ��,������ҳ���� */
	bool Initialize(const char* url = "https://github.com/frk1/hazedumper/blob/master/csgo.hpp")
	{
		/* �����ҳ·���Ƿ���Ч */
		int size = strlen(url);
		if (size <= 0) return false;

		/* ��һ�����ƹ������ľ�� */
		HINTERNET manager = InternetOpenA("CSGO Web", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
		if (manager == nullptr) return false;

		/* �������Ӹ���ҳ */
		HINTERNET target = InternetOpenUrlA(manager, url, nullptr, 0, INTERNET_FLAG_RELOAD, 0);
		if (target == nullptr)
		{
			InternetCloseHandle(manager);
			return false;
		}

		/* ����ҳ��ȫ�����ݶ�ȡ���� */
		m_data = "";
		DWORD bytes = 0;
		do
		{
			char buffer[1024]{ 0 };
			InternetReadFile(target, buffer, 1024, &bytes);
			m_data += buffer;
		} while (bytes);

		/* �رվ�� */
		InternetCloseHandle(manager);
		InternetCloseHandle(target);
		return true;
	}

	/* ��ȡָ���ֶεĵ�ַ */
	int get_address(const char* name)
	{
		/* �ֶ���Ч�Լ�� */
		int size = strlen(name);
		if (size <= 0) return 0;

		/* ��ҳ������Ч�Լ�� */
		if (m_data.empty()) return 0;

		/* �ֶβ��� */
		size_t pos = m_data.find(name);
		if (pos == std::string::npos) return 0;

		size_t beg = m_data.find("0x", pos);
		size_t end = m_data.find('<', beg);
		if (beg == ::std::string::npos || end == std::string::npos) return 0;
		std::string buf = m_data.substr(beg, end - beg);

		/* ת��Ϊʮ���������� */
		int addr = 0;
		std::stringstream translater;
		translater << buf;
		translater >> std::hex >> addr;
		return addr;
	}
};

int main(int argc, char* argv[])
{
	address* g = new address();
	if (g->Initialize())
	{
		std::cout << std::hex;

		std::cout << "������� m_dwBoneMatrix : 0x" << g->get_address("m_dwBoneMatrix") << std::endl;
		std::cout << "��ǰ���� m_hActiveWeapon : 0x" << g->get_address("m_hActiveWeapon") << std::endl;
		std::cout << "�Թ����� m_iGlowIndex : 0x" << g->get_address("m_iGlowIndex") << std::endl;

		std::cout << std::endl;

		std::cout << "��ϷĿ¼ dwClientState_MapDirectory : 0x" << g->get_address("dwClientState_MapDirectory") << std::endl;
		std::cout << "��Ϸ�Ƕ� dwClientState_ViewAngles : 0x" << g->get_address("dwClientState_ViewAngles") << std::endl;
		std::cout << "����б� dwEntityList : 0x" << g->get_address("dwEntityList") << std::endl;
	}
	system("pause");
	return 0;
}