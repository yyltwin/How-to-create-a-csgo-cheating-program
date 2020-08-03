#pragma once
#include <Windows.h>

constexpr int byte_length = 5;

class inline_hook
{
private:
	using uchar = unsigned char;
	using dword = DWORD;

	uchar m_original_byte[byte_length];//ԭʼ�������Ļ��ָ��
	uchar m_self_byte[byte_length];//���ǹ���Ļ��ָ��

	int m_original_address;//ԭʼ�����ĵ�ַ
	int m_self_address;//���Ǻ����ĵ�ַ

	dword motify_memory_attributes(int address, dword attributes = PAGE_EXECUTE_READWRITE)
	{
		dword old_attributes;
		VirtualProtect(reinterpret_cast<void*>(address), byte_length, attributes, &old_attributes);
		return old_attributes;
	}

public:
	inline_hook(int original_address, int self_address) : m_original_address(original_address), m_self_address(self_address)
	{
		
		m_self_byte[0] = '\xe9';

		//����ƫ��
		int offset = self_address - (original_address + byte_length);

		//������ת�����Ǻ�����byte
		memcpy(&m_self_byte[1], &offset, byte_length - 1);

		//�޸��ڴ�����
		dword attributes = motify_memory_attributes(original_address);

		//����ԭʼ������ַ��byte
		memcpy(m_original_byte, reinterpret_cast<void*>(original_address), byte_length);

		//�ָ��ڴ�����
		motify_memory_attributes(original_address, attributes); 
	}

	void motify_address()
	{
		dword attributes = motify_memory_attributes(m_original_address);

		//д�����ǹ����byteʵ��inline hook
		memcpy(reinterpret_cast<void*>(m_original_address), m_self_byte, byte_length);

		motify_memory_attributes(m_original_address, attributes);
	}

	void restore_address()
	{
		dword attributes = motify_memory_attributes(m_original_address);

		memcpy(reinterpret_cast<void*>(m_original_address), m_original_byte, byte_length);

		motify_memory_attributes(m_original_address, attributes);
	}

};


