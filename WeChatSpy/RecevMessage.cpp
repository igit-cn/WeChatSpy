// RecvMessage.cpp : ���� DLL Ӧ�ó���ĵ���������
// ��ȡ΢�Ž��յ����ı���Ϣ��������Server��
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include <TlHelp32.h>
#include "utils.h"
#include <stdlib.h>
#include <WINSOCK2.H>
#include "socketTool.h"
#pragma comment(lib, "ws2_32.lib")
#define HOOK_LEN 5

//SOCKET rece_client = 0;   //socket�׽���
HANDLE hWHND = 0;
BYTE backCode[HOOK_LEN] = { 0 };
DWORD WinAdd = 0;
DWORD retAdd = 0;
HWND hDlg = 0;

VOID printLog(DWORD msgAdd);
VOID send_to_py_server(DWORD msgAdd);
VOID StartHookWeChat(HWND hwndDlg, DWORD hookAdd, SOCKET client);

/*
esi + 0x178 ��Ϣ����
esi + 0x1A0 ����Ǹ�����Ϣ���Ǹ���wxid�����Ⱥ��Ϣ����Ⱥid
esi + 0xCC �����Ⱥ��Ϣ������ΪȺ�﷢����Ϣ�˵�wxid��֮Ϊ0x0
*/
//��ʾ����
VOID printLog(DWORD msgAdd)
{
	DWORD wxidAdd = msgAdd - 0x1B8;
	DWORD wxid2Add = msgAdd - 0xCC;
	DWORD messageAdd = msgAdd - 0x190;
	wchar_t buff[0x1000] = { 0 };
	GetDlgItemText(hDlg, MESSAGE, buff, sizeof(buff));
	if (*(LPVOID *)wxid2Add <= 0x0) {
		swprintf_s(buff, L"wxid: %s\r\n��Ϣ����:\r\n%s", *((LPVOID *)wxidAdd), *((LPVOID *)messageAdd));
		//swprintf_s(buff, L"ESI=%p wxid=%p wxid2=%p wxid2=%p\r\n", msgAdd, msgAdd - 0x1A0, *((LPVOID *)wxidAdd));
	}
	else {
		swprintf_s(buff, L"ȺID: %s\r\n������ID: %s\r\n��Ϣ����:\r\n%s", *((LPVOID *)wxidAdd), *((LPVOID *)wxid2Add), *((LPVOID *)messageAdd));
	}

	SetDlgItemText(hDlg, MESSAGE, buff);
}

// �����ݷ��͸�python������
VOID send_to_py_server(DWORD msgAdd)
{
	DWORD wxidAdd = msgAdd - 0x1B8;     //��ַָ�룬����Ⱥ��Ϣ��������Ⱥid�����Ǹ�����Ϣ�����Ǹ���id
	DWORD wxid2Add = msgAdd - 0xCC;     //��ַָ�룬����Ⱥ��Ϣ�������Ƿ���Ⱥ��Ϣ���˵�id�����Ǹ�����Ϣ��������0x0
	DWORD messageAdd = msgAdd - 0x190;  //��ַָ�룬��Ϣ����
	DWORD msg_type_add = msgAdd - 0x1C8;   //��ַ, ��Ϣ����
	DWORD self_add = msgAdd - 0x1C4;       //��ַ����Ϣ��Դ���Լ������Ǳ��˷���

	wchar_t msg_type[0x100] = {0};
	wchar_t self[0x100] = {0};

	DWORDToUnicode(*((DWORD *)msg_type_add), msg_type);  //��Ϣ����
	DWORDToUnicode(*((DWORD *)self_add), self);  //��Ϣ��Դ���Լ������Ǳ��˷���

	wchar_t buff[0x1000] = { 0 };
	wchar_t type[0x100] = L"1";  //��Ϣ���ͣ�1�����͸�python server�����ı���Ϣ������python server�����ݵĽ���
	////��ȡ΢�Ž���pid
	CHAR pid_str[0x100] = { 0 };
	DWORD PID = GetCurrentProcessId();  //��ȡ΢�Ž���pid
	// ��DWORD(��int)����ת��wchat_t����
	_itoa_s(PID, pid_str, 10);
	wchar_t processPid[0x100] = { 0 };
	//get_process_pid(processPid); //��ȡ΢�Ž���pid�� GetCurrentProcessId�����������ļ�����
	swprintf(processPid, sizeof(processPid), L"%hs", pid_str);

	//MessageBox(NULL, (LPCWSTR)processPid, L"ProcessId", 0);
	if (*(LPVOID *)wxid2Add <= 0x0) {
		//��ȡ������Ϣ
		swprintf_s(buff, L"{\"pid\":%s,\"self\":%s,\"type\":%s,\"msg_type\":%s,\"chatroom_ID\":\"%s\",\"wx_ID\":\"%s\",\"content\":\"%s\"}*393545857*",
			processPid, self, type, msg_type, L"", *((LPVOID *)wxidAdd), *((LPVOID *)messageAdd));
	}
	else {
		//��ȡȺ��Ϣ
		swprintf_s(buff, L"{\"pid\":%s,\"self\":%s,\"type\":%s,\"msg_type\":%s,\"chatroom_ID\":\"%s\",\"wx_ID\":\"%s\",\"content\":\"%s\"}*393545857*",
			processPid, self, type, msg_type, *((LPVOID *)wxidAdd), *((LPVOID *)wxid2Add), *((LPVOID *)messageAdd));
	}
	//MessageBox(NULL, buff, L"Message", 0);

	if (Global_Client == 0)
		return;

	const char * sendData = UnicodeToChar(buff);  //��Unicode����ת��CHAR���ͣ�����socket����
    //send()������������ָ����socket�����Է�����
	//int send(int s, const void * msg, int len, unsigned int flags)
	//sΪ�ѽ��������ӵ�socket��msgָ���������ݣ�len��Ϊ���ݳ��ȣ�����flagsһ����0
	//�ɹ��򷵻�ʵ�ʴ��ͳ�ȥ���ַ�����ʧ�ܷ���-1������ԭ�����error 
	send(Global_Client, sendData, strlen(sendData), 0);
	//closesocket(sclient);
}


DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;

//��ת�����ĺ��� �����Լ���
VOID __declspec(naked) HookF()
{
	//pushad: �����е�32λͨ�üĴ���ѹ���ջ
	//pushfd:Ȼ��32λ��־�Ĵ���EFLAGSѹ���ջ
	//popad:�����е�32λͨ�üĴ���ȡ����ջ
	//popfd:��32λ��־�Ĵ���EFLAGSȡ����ջ
	//�ȱ���Ĵ���
	// ʹ��pushad��Щ���㻹�ǲ�̫�ȶ�  �����ñ����ѼĴ�����ֵ�������� �����ɿ���
	__asm {
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi

		pushad
		pushfd
	}
	//Ȼ����ת�������Լ��Ĵ�����
	//��ʾ���յ�����Ϣ����
	//printLog(cEsi);
	//�����ݷ���server
	send_to_py_server(cEsi);
	retAdd = WinAdd + 0x355618;
	//Ȼ���ڻ�ԭ������֮ǰ����������
	/*popad
		popfd  ��̫�ɿ��ָ���ȫ ���Բ��б����ķ�ʽ���������ٸ�ֵ��ȥ*/
	__asm {
		/*mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi
		mov eax, retCallAdd
		call retCallAdd2*/
		popfd    //�ѱ��ݵ����ݻָ�
		popad
		jmp retAdd
	}
}

VOID StartHookWeChat()
{
	DWORD hookAdd = getWechatWin() + 0x355613;
	hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	WinAdd = getWechatWin();
	//hDlg = hwndDlg;
	LPVOID jmpAdd = &HookF;

	BYTE JmpCode[HOOK_LEN] = { 0 };
	//������Ҫ���һ������������
	// E9 11051111(��������ת�ĵط�����ط�����һ�������ַ ���Ǹ���hook��ַ����ת�Ĵ����ַ�ľ�����������)
	JmpCode[0] = 0xE9;
	//������ת�ľ��빫ʽ�ǹ̶���
	//���㹫ʽΪ ��ת�ĵ�ַ(Ҳ�������Ǻ����ĵ�ַ) - hook�ĵ�ַ - hook���ֽڳ���
	*(DWORD *)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - HOOK_LEN;

	//hook�ڶ��� �ȱ��ݽ�Ҫ�����Ǹ��ǵ�ַ������ ����Ϊ����hook�ĳ��� HOOK_LEN 5���ֽ�

	//��ȡ���̾��

	wchar_t debugBuff[0x100] = { 0 };
	swprintf_s(debugBuff, L"hook��ַ=%p  ���̾��=%p  jmp����=%p  AA=%p", hookAdd, hWHND, jmpAdd, &HookF);
	//MessageBox(NULL, debugBuff, L"����", MB_OK);
	//��������
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {

		swprintf_s(debugBuff, L"hook��ַ=%p  ���̾��=%p  ��������=%d", hookAdd, hWHND, GetLastError());
		MessageBox(NULL, debugBuff, L"��ȡʧ��", MB_OK);
		//MessageBox(NULL,"hook��ַ�����ݶ�ȡʧ��","��ȡʧ��",MB_OK);
		return;
	}

	//������hook��ʼ�� ������Ҫ�滻�ĺ�����ַд��ȥ ����ֱ���������Ǻ�������ȥȻ�����Ǵ�����Ϻ��ٷ��аɣ�
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hookд��ʧ�ܣ������滻ʧ��", L"����", MB_OK);
		return;
	}
	//MessageBox(NULL, L"�ɹ�HOOK", L"�ɹ�", MB_OK);
}