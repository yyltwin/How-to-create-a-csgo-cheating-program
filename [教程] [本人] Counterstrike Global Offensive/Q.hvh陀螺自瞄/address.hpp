#pragma once

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
			static const int len = 1024 * 10;
			char buffer[len]{ 0 };
			InternetReadFile(target, buffer, len, &bytes);
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