#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "socketTool.h"
#include "resource.h"


//��ȡ������Ϣ
VOID readWechatData()
{
	wchar_t buff[0x5000] = { 0 };

	// ��ȡWeChatWin.dllģ���ַ
	DWORD WinAdd = getWechatWin();

	//��ȡ΢��ID
	wchar_t wxid[0x100] = { 0 };  //װ���ݵ�����
	DWORD pWXID = WinAdd + 0x1620D50;
	swprintf(wxid, sizeof(wxid), L"%hs", *(DWORD *)pWXID);  //CHARת��wchar_t

	//��ȡ΢���˺�
	wchar_t account[0x100] = { 0 };  //װ���ݵ�����
	swprintf(account, sizeof(account), L"%hs", WinAdd + 0x1620AB0);

	//��ȡ΢��ͷ��   
	wchar_t avatar[0x300] = { 0 };  //װ���ݵ�����
	DWORD pAvatar = WinAdd + 0x1620C14;
	swprintf(avatar, sizeof(avatar), L"%hs", *(DWORD *)pAvatar);

	//��ȡ�ֻ���
	wchar_t phone[0x100] = { 0 };  //װ���ݵ�����
	swprintf(phone, sizeof(phone), L"%hs", WinAdd + 0x1620980);


	//��ȡʡ��
	wchar_t province[0x100] = { 0 };  //װ���ݵ�����
	//swprintf_s(province, L"%s", WinAdd + 0x1620A38);
	swprintf(province, sizeof(province), L"%hs", WinAdd + 0x1620A38);

	//��ȡ����
	wchar_t city[0x100] = { 0 };  //װ���ݵ�����
	//swprintf_s(city, L"%s", WinAdd + 0x1620A50);
	swprintf(city, sizeof(city), L"%hs", WinAdd + 0x1620A50);

	//��ȡ�豸��Ϣ
	wchar_t device[0x100] = { 0 };  //װ���ݵ�����
	//swprintf_s(device, L"%s", WinAdd + 0x1620D88);
	swprintf(device, sizeof(device), L"%hs", WinAdd + 0x1620D88);

	////��ȡ����ǩ��   
	//wchar_t * signature = NULL;  //װ���ݵ�����
	//CHAR signature_char[0x100] = {0};
	//sprintf_s(signature_char, "%s", WinAdd + 0x1620A68);
	//signature = UTF8ToUnicode(signature_char);


	//��ȡ΢���ǳ�
	wchar_t * nickname = NULL;  //װ���ݵ�����
	CHAR nickname_char[0x200] = { 0 };
	sprintf_s(nickname_char, "%s", WinAdd + 0x162094C);
	nickname = UTF8ToUnicode(nickname_char);


	//��ȡ΢�Ž���pid
	CHAR pid_str[0x100] = { 0 };
	DWORD PID = GetCurrentProcessId();  //��ȡ΢�Ž���pid
	// ��DWORD(��int)����ת��CHAR����
	_itoa_s(PID, pid_str, 10);
	wchar_t processPid[0x100] = { 0 };
	//get_process_pid(processPid); //��ȡ΢�Ž���pid�� GetCurrentProcessId�����������ļ�����
	swprintf(processPid, sizeof(processPid), L"%hs", pid_str);  //CHARת��wchar_t


	//ƴ������
	wchar_t type[0x100] = L"2";  //��Ϣ���ͣ�2�����͸�python server���Ǹ�����Ϣ
	swprintf_s(buff, L"{\"pid\":%s,\"type\":%s,\"wx_ID\":\"%s\",\"nickname\":\"%s\" \
		,\"account\":\"%s\",\"phone\":\"%s\",\"province\":\"%s\",\"city\":\"%s\" \
		,\"device\":\"%s\",\"avatar\":\"%s\"}*393545857*",
		processPid, type, wxid, nickname, account, phone, province, city, device, avatar);

	//swprintf_s(buff, L"{\"pid\":%s,\"type\":%s,\"nickname\":\"%s\"}",
	//	processPid, type, nickname);

	const char * sendData = UnicodeToChar(buff);  //��Unicode����ת��CHAR���ͣ�����socket����
	//send()������������ָ����socket�����Է�����
	//int send(int s, const void * msg, int len, unsigned int flags)
	//sΪ�ѽ��������ӵ�socket��msgָ���������ݣ�len��Ϊ���ݳ��ȣ�����flagsһ����0
	//�ɹ��򷵻�ʵ�ʴ��ͳ�ȥ���ַ�����ʧ�ܷ���-1������ԭ�����error 
	send(Global_Client, sendData, strlen(sendData), 0);
	//closesocket(sclient);
}