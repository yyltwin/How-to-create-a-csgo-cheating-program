/*
CSGOע��С����
ԭ�� : �޸�NtOpenFile������inline hook
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <filesystem>

#include <Windows.h>
#include <TlHelp32.h>

/* ��ȡCSGO���̾�� */
HANDLE GetProcessHandle()
{
	/* �򿪱������̾�� */
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Snap == INVALID_HANDLE_VALUE) return Snap;

	/* ��ʼ��������Ϣ�ṹ */
	PROCESSENTRY32 ProcessInfo{ 0 };
	ProcessInfo.dwSize = sizeof(ProcessInfo);

	/* ѭ�����ҽ�����Ϣ */
	if (Process32First(Snap, &ProcessInfo))
	{
		do
		{
			if (strcmp("csgo.exe", ProcessInfo.szExeFile) == 0)
			{
				CloseHandle(Snap);
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.th32ProcessID);
			}
		} while (Process32Next(Snap, &ProcessInfo));
	}

	CloseHandle(Snap);
	return INVALID_HANDLE_VALUE;
}

int main(int argc, char* argv[])
{
	HANDLE Handle = INVALID_HANDLE_VALUE;
	LPVOID DllMemory = nullptr;
	HANDLE Thread = NULL;

	do
	{
		/* ��ȡ����DLL·�� filesystem����������C++17�汾*/
		std::cout << "����DLLȫ·�� : ";
		std::string dll;
		getline(std::cin, dll);
		if (std::filesystem::exists(dll) == false)
		{
			std::cout << "[-] DLL�ļ�������" << std::endl;
			break;
		}

		/* ��ȡCSGO���̾�� */
		Handle = GetProcessHandle();
		if (Handle == INVALID_HANDLE_VALUE)
		{
			std::cout << "[-] ��ȡ��Ϸ���̾��ʧ��" << std::endl;
			break;
		}

		/* ��ȡNtOpenFile������ַ */
		LPVOID pNtOpenFile = GetProcAddress(LoadLibraryA("ntdll"), "NtOpenFile");
		if (pNtOpenFile == nullptr)
		{
			std::cout << "[-] ��ȡNtOpenFile������ַʧ��" << std::endl;
			break;
		}

		/* ��ȡԭʼ�ֽ� */
		char OriginalBytes[5]{ 0 };
		memcpy(OriginalBytes, pNtOpenFile, 5);

		/* ��ԭʼ�ֽ�д��CSGO���̵�NtOpenFile������ַ��,���inline hook */
		SIZE_T Bytes = 0;
		WriteProcessMemory(Handle, pNtOpenFile, OriginalBytes, 5, &Bytes);
		if (Bytes == 0)
		{
			std::cout << "[-] �޷�д���ڴ���inline hook" << std::endl;
			break;
		}

		/* ��CSGO��Ϸ������һ���ڴ汣�����ǵ�DLL·�� */
		DllMemory = VirtualAllocEx(Handle, nullptr, dll.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (DllMemory == nullptr)
		{
			std::cout << "[-] �޷���CSGO��Ϸ�������ڴ�" << std::endl;
			break;
		}

		/* �����ǵ�DLL·��д��ÿ��ڴ��� */
		WriteProcessMemory(Handle, DllMemory, dll.c_str(), dll.size(), &Bytes);
		if (Bytes == 0)
		{
			std::cout << "[-] �޷�д��DLL·��" << std::endl;
			break;
		}

		/* ����Զ���̼߳������ǵ�DLL */
		Thread = CreateRemoteThread(Handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, DllMemory, 0, 0);
		if (Thread == NULL)
		{
			std::cout << "[-] �޷�����Զ���߳�" << std::endl;
			break;
		}

		/* �ȴ�Զ���̵߳�ִ����� */
		WaitForSingleObject(Thread, INFINITE);

		/* ���سɹ� */
		std::cout << "[+] ע��ɹ�" << std::endl;
	} while (false);

	/* ������� */
	if (Handle != INVALID_HANDLE_VALUE) CloseHandle(Handle);
	if (DllMemory) VirtualFree(DllMemory, 0, MEM_RELEASE);
	if (Thread != NULL) CloseHandle(Thread);

	system("pause");
	return 0;
}