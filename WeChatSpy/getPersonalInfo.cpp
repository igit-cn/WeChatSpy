#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "socketTool.h"
#include "resource.h"


//读取个人信息
VOID readWechatData()
{
	wchar_t buff[0x5000] = { 0 };

	// 获取WeChatWin.dll模块基址
	DWORD WinAdd = getWechatWin();

	//获取微信ID
	wchar_t wxid[0x100] = { 0 };  //装数据的容器
	DWORD pWXID = WinAdd + 0x1620D50;
	swprintf(wxid, sizeof(wxid), L"%hs", *(DWORD *)pWXID);  //CHAR转成wchar_t

	//获取微信账号
	wchar_t account[0x100] = { 0 };  //装数据的容器
	swprintf(account, sizeof(account), L"%hs", WinAdd + 0x1620AB0);

	//获取微信头像   
	wchar_t avatar[0x300] = { 0 };  //装数据的容器
	DWORD pAvatar = WinAdd + 0x1620C14;
	swprintf(avatar, sizeof(avatar), L"%hs", *(DWORD *)pAvatar);

	//获取手机号
	wchar_t phone[0x100] = { 0 };  //装数据的容器
	swprintf(phone, sizeof(phone), L"%hs", WinAdd + 0x1620980);


	//获取省份
	wchar_t province[0x100] = { 0 };  //装数据的容器
	//swprintf_s(province, L"%s", WinAdd + 0x1620A38);
	swprintf(province, sizeof(province), L"%hs", WinAdd + 0x1620A38);

	//获取城市
	wchar_t city[0x100] = { 0 };  //装数据的容器
	//swprintf_s(city, L"%s", WinAdd + 0x1620A50);
	swprintf(city, sizeof(city), L"%hs", WinAdd + 0x1620A50);

	//获取设备信息
	wchar_t device[0x100] = { 0 };  //装数据的容器
	//swprintf_s(device, L"%s", WinAdd + 0x1620D88);
	swprintf(device, sizeof(device), L"%hs", WinAdd + 0x1620D88);

	////获取个性签名   
	//wchar_t * signature = NULL;  //装数据的容器
	//CHAR signature_char[0x100] = {0};
	//sprintf_s(signature_char, "%s", WinAdd + 0x1620A68);
	//signature = UTF8ToUnicode(signature_char);


	//获取微信昵称
	wchar_t * nickname = NULL;  //装数据的容器
	CHAR nickname_char[0x200] = { 0 };
	sprintf_s(nickname_char, "%s", WinAdd + 0x162094C);
	nickname = UTF8ToUnicode(nickname_char);


	//获取微信进程pid
	CHAR pid_str[0x100] = { 0 };
	DWORD PID = GetCurrentProcessId();  //获取微信进程pid
	// 把DWORD(即int)类型转成CHAR类型
	_itoa_s(PID, pid_str, 10);
	wchar_t processPid[0x100] = { 0 };
	//get_process_pid(processPid); //获取微信进程pid， GetCurrentProcessId不能在其他文件调用
	swprintf(processPid, sizeof(processPid), L"%hs", pid_str);  //CHAR转成wchar_t


	//拼接数据
	wchar_t type[0x100] = L"2";  //消息类型，2代表发送给python server的是个人信息
	swprintf_s(buff, L"{\"pid\":%s,\"type\":%s,\"wx_ID\":\"%s\",\"nickname\":\"%s\" \
		,\"account\":\"%s\",\"phone\":\"%s\",\"province\":\"%s\",\"city\":\"%s\" \
		,\"device\":\"%s\",\"avatar\":\"%s\"}*393545857*",
		processPid, type, wxid, nickname, account, phone, province, city, device, avatar);

	//swprintf_s(buff, L"{\"pid\":%s,\"type\":%s,\"nickname\":\"%s\"}",
	//	processPid, type, nickname);

	const char * sendData = UnicodeToChar(buff);  //将Unicode编码转成CHAR类型，用于socket传输
	//send()用来将数据由指定的socket传给对方主机
	//int send(int s, const void * msg, int len, unsigned int flags)
	//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
	//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error 
	send(Global_Client, sendData, strlen(sendData), 0);
	//closesocket(sclient);
}