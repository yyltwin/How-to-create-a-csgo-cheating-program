#pragma once

#include <Windows.h>
#include <TlHelp32.h>

class memory
{
private:
	DWORD m_pid;//����ID

public:
	memory() {}
	~memory() {}

	/* ��ȡ����ID */
	inline DWORD get_pid() { return m_pid; }

	/* ����ָ������ */
	bool attach(const char* process)
	{
		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Snap == INVALID_HANDLE_VALUE) return false;

		PROCESSENTRY32 ProcessInfo{ 0 };
		ProcessInfo.dwSize = sizeof(ProcessInfo);

		if (Process32First(Snap, &ProcessInfo))
		{
			do
			{
				if (strcmp(process, ProcessInfo.szExeFile) == 0)
				{
					CloseHandle(Snap);
					m_pid = ProcessInfo.th32ProcessID;
					return true;
				}
			} while (Process32Next(Snap, &ProcessInfo));
		}

		CloseHandle(Snap);
		return false;
	}

	/* ����ָ��ģ�� */
	MODULEENTRY32 find_module(const char* name)
	{
		MODULEENTRY32 Result{ 0 };
		Result.dwSize = sizeof(Result);

		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_pid);
		if (Snap == INVALID_HANDLE_VALUE) return Result;

		if (Module32First(Snap, &Result))
		{
			do
			{
				if (strcmp(name, Result.szModule) == 0)
				{
					CloseHandle(Snap);
					return Result;
				}
			} while (Module32Next(Snap, &Result));
		}

		CloseHandle(Snap);
		return {};
	}

	/* ��ȡ�ڴ� */
	template<class T>
	T read(DWORD addr, int size = 0)
	{
		T result{};
		size = (size == 0) ? sizeof(result) : size;

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
		if (hProcess == NULL) return result;

		SIZE_T bytes = 0;
		ReadProcessMemory(hProcess, (LPCVOID)addr, &result, size, &bytes);

		CloseHandle(hProcess);

		return result;
	}

	/* ��ȡ�ַ��� */
	std::string read_string(DWORD addr, int size)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
		if (hProcess == NULL) return "";

		SIZE_T bytes = 0;
		std::string buffer;
		buffer.resize(size);
		ReadProcessMemory(hProcess, (LPCVOID)addr, buffer.data(), size, &bytes);

		CloseHandle(hProcess);

		return buffer;
	}

	/* д���ڴ� */
	template<class T>
	void write(DWORD addr, T buf, int size = 0)
	{
		size = (size == 0) ? sizeof(buf) : size;

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
		if (hProcess == NULL) return;

		SIZE_T bytes = 0;
		WriteProcessMemory(hProcess, (LPVOID)addr, &buf, size, &bytes);

		CloseHandle(hProcess);
	}
};