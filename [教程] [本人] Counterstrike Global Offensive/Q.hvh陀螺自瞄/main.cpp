#pragma warning(disable : 4244)

#include "cheats.hpp"
#include <process.h>

//�����߳�
void __cdecl hack_thread(void*)
{
	cheats* g = new cheats();
	if (g->initialize() == false) return;

	// ֵ����
	g->handle_convar();

	while (true)
	{
		// ���µ�ͼ
		g->update_map();

		// ��Ĭ����
		if (g->silent() == false) {}//g->big_top();

		// �Թ�
		g->glow();

		// ����
		g->bunnyhop();

		Sleep(10);
	}
}

int main(int argc, char* argv[])
{
	HANDLE hThread = (HANDLE)_beginthread(hack_thread, 0, nullptr);
	WaitForSingleObject(hThread, INFINITE);
	system("pause");
	return 0;
}