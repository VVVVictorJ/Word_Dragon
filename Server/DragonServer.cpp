// DragonServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include<iostream>

/**********************************************************************************\
|*	Module:																		  *|
|*	Notices:Copyright(c) 2018 Victor Robin                                        *|
|*	Purpose:																	  *|
\**********************************************************************************/

/**************************************分割线*****************************************/

//缺
	//广播线程
	//线程间的通信//不需要
	//时间模块✔
	//记录每个socket的具体时间
	//使用hashmap排序

/**************************************分割线*****************************************/

struct imformationclient
{
	char address[20];//记录地址方便回传

}imc;

/**************************************分割线*****************************************/
#pragma region Gloabl_variable
SOCKET clientsocket;//客户端套接字
SOCKET broadcast;//广播套接字
SOCKET servSocket;
sockaddr_in baddr;
enum { MAX = 200 };
HANDLE mutex;//互斥量
bool IsServeRunning;
std::list<CClient*> clientlist;//与服务器建立的各种连接
std::vector<CClient*> clientvector;
std::vector<SOCKET>clientGroup;//多线程存储结构
std::hash_map<double, CClient*>serialist;//按时间存储的hashmap
std::vector<char*>addrr;
const int MAX_BUF_LEN = 255;
#pragma endregion
/**************************************分割线*****************************************/
#pragma region 多余函数
template<typename T>
std::string toString(const T & t)
{
	std::ostringstream s;
	s << t;
	return s.str();
}
const char *allocCat(const char * const s1, const char * const s2)
{
	char* str = (char*)malloc(strlen(s1) + strlen(s2) + 1);
	strcpy(str, s1);
	strcat(str, s2);
	return str;
}
#pragma endregion
/**************************************分割线*****************************************/
#pragma region fuction0
bool Initmember();
bool Initsocket();
bool InitBroadSocket();
#pragma endregion

#pragma region 多余函数3
DWORD WINAPI SendMessageThread(LPVOID IpParameter);
DWORD WINAPI RecvMessageThread(LPVOID IpParameter);
DWORD WINAPI AcceptTread(LPVOID IpParameter);
DWORD WINAPI Broad_Thread(LPVOID IpParameter);
#pragma endregion
/**************************************分割线*****************************************/
int _tmain(int argc, TCHAR* argv[])
{
#pragma region waste
	//if (TRUE != Initsocket())
	//{
	//	std::cout << "Initsocket fasle" << std::endl;
	//	return 0;
	//}
	//mutex = CreateSemaphore(NULL, 1, 1, NULL);
	//HANDLE accepthread = CreateThread(NULL, 0, AcceptTread, NULL, 0, NULL);
	//CloseHandle(accepthread);
	//system("pause");
#pragma endregion

#pragma region In_it
	Initmember();
	InitBroadSocket();
	Initsocket();
#pragma endregion

#pragma region t_parameter
	clock_t start, finished;
	start = clock();

	clock_t record;
#pragma endregion
	while (IsServeRunning)
	{
#pragma region parameter
		//-----------------------------参数--------------------------------//
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		SOCKET s = INVALID_SOCKET;
		
		int len = sizeof(addr);
		int i = 1;
		s = accept(servSocket, (sockaddr*)&addr, &len);
		finished = clock();
		//-----------------------------参数--------------------------------//
#pragma endregion

#pragma region cal_time
		//-----------------------------计时--------------------------------//
		//120s结束接受注册
		if ((finished - start) / CLOCKS_PER_SEC > 20)
		{
			std::cout << "time has consumed.." << std::endl;
			//广播游戏即将开始
			IsServeRunning = false;
			break;
		}
		//-----------------------------计时--------------------------------//
#pragma endregion

#pragma region jud_
		//-----------------------------判断--------------------------------//
		if (SOCKET_ERROR == s)
		{
			int r = WSAGetLastError();
			if (WSAEWOULDBLOCK == r)
			{
				Sleep(1000);
				continue;
			}
			else
			{
				std::cout << "未知错误" << std::endl;
				continue;
			}
		}
		//-----------------------------判断--------------------------------//
#pragma endregion
		else
		{
			std::cout << inet_ntoa( addr.sin_addr)<<" come in.." << std::endl;
			addrr.push_back(inet_ntoa(addr.sin_addr));
			char buff[1024];
			memset(buff, 0, 1024);
			CClient* pClient = new CClient(s, addr);
			pClient->startRunning();
			clientvector.push_back(pClient);
		}

	}
	char tmp[30] = "GAME WILL BE STARTED.";
	HANDLE bbb = CreateThread(NULL, 0, Broad_Thread, (void*)tmp, 0, NULL);
	CloseHandle(bbb);
	//按时间排序
	//执行轮询
		//若错误剔除出 hashmap
		//并且广播剔除出的ip
	std::vector<CClient*>::iterator iter;
	CClient* ccc;
	iter = clientvector.begin();
	for (int i = 0; i < clientvector.size(); i++)
	{
		clientvector[i]->startSending();
		clientvector[i]->startRunning_1();
		Sleep(10000);
		if (false == clientvector[i]->version)
		{
			clientvector.erase(iter + i);
			iter = clientvector.begin();
		}

	}
	//std::sort(serialist.begin(), serialist.end());
	//std::hash_map<double, CClient*>::iterator iter;
	//while (clientlist.size() != 0)
	//{
	//	iter = serialist.begin();
	//	while (iter != serialist.end())
	//	{

	//		iter++;
	//	}
	//}
	Sleep(5000);
	return std::cin.get();
}
/**************************************分割线*****************************************/
#pragma region function

#pragma region Initmember
bool Initmember()
{
	IsServeRunning = true;
	servSocket = INVALID_SOCKET;
	broadcast = INVALID_SOCKET;
	std::cout << "Initialization down.." << std::endl;
	return 0;
}
#pragma endregion

#pragma region InitBroadSocket
bool InitBroadSocket()
{
	WORD wVersionRequested;
	WSADATA wasData;//指向WinSocket信息结构的指针
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	// 初始化winsock库，使用socket的前提
	err = WSAStartup(wVersionRequested, &wasData);
	if (err != 0)
	{
		return -1;
	}
	if (LOBYTE(wasData.wVersion) != 2 || HIBYTE(wasData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}
	SOCKET broadcast = socket(AF_INET, SOCK_DGRAM, 0);
	if (broadcast == INVALID_SOCKET)
	{
		err = WSAGetLastError();
		printf("socket error:%d\n", err);
		closesocket(broadcast);
		WSACleanup();
		return -1;
	}
	bool bOpt = true;
	//打开广播选项
	setsockopt(broadcast, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	// 设置发往的地址
	SOCKADDR_IN baddr;
	memset(&baddr, 0, sizeof(baddr));
	baddr.sin_family = AF_INET;
	baddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
	baddr.sin_port = htons(6666);
	char buff[MAX_BUF_LEN] = { "你妈死了叶长青" };
	memset(buff, 0, MAX_BUF_LEN);
	int nSendSize = sendto(broadcast, buff, strlen(buff), 0, (SOCKADDR*)&baddr, sizeof(SOCKADDR_IN));
	std::cout << "module of broadcast done.." << std::endl;
	if (nSendSize == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		printf("sendto error:%d\n", err);
		closesocket(broadcast);
		WSACleanup();
		return -1;
	}
	return true;
}

#pragma endregion

#pragma region Initsocket
bool Initsocket()
{
	WSAData wsadata;
	int iResult;
	unsigned long ul = 1;
	//----------------------------------------------------------------//
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

	if (NO_ERROR != iResult)
	{
		std::cout << "Error at WSAStartup()" << std::endl;
	}
	servSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == servSocket)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	//----------------------------------------------------------------//

	//非阻塞模式
	iResult = ioctlsocket(servSocket, FIONBIO, &ul);

	if (NO_ERROR != iResult)
	{
		printf("ioctlsocket failed with error: %ld\n", iResult);
	}

	//----------------------------------------------------------------//
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	memset(&baddr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	//addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//broadcast
	baddr.sin_family = AF_INET;
	baddr.sin_port = htons(8878);
	baddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	if (SOCKET_ERROR == bind(servSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR)))
	{
		std::cout << "Error at Bind()" << std::endl;
	}

	iResult = listen(servSocket, 20);
	if (-1 == iResult)
	{
		printf("listen failed with error: %ld\n", WSAGetLastError());
	}

	std::cout << "server is uping.." << std::endl;

	return TRUE;
}
#pragma endregion

#pragma endregion

#pragma region 多余函数2
DWORD WINAPI SendMessageThread(LPVOID IpParameter)
{
	while (TRUE)
	{
		std::string str;
		std::getline(std::cin, str);
		WaitForSingleObject(mutex, INFINITE);//source isn't be occupied
		for (int i = 0; i < (int)clientGroup.size(); ++i)
		{
			send(clientsocket, str.c_str(), 200, 0);//send message
		}
		ReleaseSemaphore(mutex, 1, NULL);
	}
	return TRUE;
}
DWORD WINAPI RecvMessageThread(LPVOID IpParameter)
{
	SOCKET CClient = (SOCKET)(LPVOID)IpParameter;
	while (TRUE)
	{
		char recvbuf[MAX];
		recv(CClient, recvbuf, 200, 0);
		WaitForSingleObject(mutex, INFINITE);

		if (0 == strcmp(recvbuf, "exit"))// the thread that receive the “exit” will be erased.
		{
			std::vector<SOCKET>::iterator it = std::find(clientGroup.begin(), clientGroup.end(), CClient);
			clientGroup.erase(it);
			closesocket(CClient);
			ReleaseSemaphore(mutex, 1, NULL);
			std::cout << "Kill one thread.." << std::endl;
			break;
		}
		printf("%s said:\t%s\n.", "someone", recvbuf);//someone said what.
		ReleaseSemaphore(mutex, 1, NULL);//release mutex.
	}
	return TRUE;
}
DWORD WINAPI AcceptTread(LPVOID IpParameter)
{
	HANDLE sendThread = CreateThread(NULL, 0, SendMessageThread, NULL, 0, NULL);
	while (TRUE)
	{
		sockaddr_in addr;
		SOCKET s;
		int len = sizeof(addr);
		s = accept(clientsocket, (sockaddr*)&addr, &len);
		if (SOCKET_ERROR == s)
		{
			int r = WSAGetLastError();
			if (WSAEWOULDBLOCK == r)
			{
				Sleep(1000);
				continue;
			}
			else
			{
				std::cout << "未知错误，接受客户端连接线程退出。" << std::endl;
				getchar();
				return false;
			}
		}
		else
		{
			clientGroup.push_back(s);
		}
		HANDLE receiveThread = CreateThread(NULL, 0, RecvMessageThread, (LPVOID)s, 0, NULL);
		WaitForSingleObject(mutex, INFINITE);
		if (NULL == receiveThread)
		{
			printf("\nCreatThread AnswerThread() failed.\n");
		}
		else
		{
			printf("\nCreate Receive Client Thread OK.\n");
		}
		ReleaseSemaphore(mutex, 1, NULL);
	}
	WaitForSingleObject(sendThread, INFINITE);
	CloseHandle(sendThread);
	CloseHandle(mutex);
	WSACleanup();
	return TRUE;
}
DWORD __stdcall Broad_Thread(LPVOID IpParameter)
{

	WORD wVersionRequested;
	WSADATA wasData;//指向WinSocket信息结构的指针
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	// 初始化winsock库，使用socket的前提
	err = WSAStartup(wVersionRequested, &wasData);
	if (err != 0)
	{
		return -1;
	}
	if (LOBYTE(wasData.wVersion) != 2 || HIBYTE(wasData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}
	SOCKET conn_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (conn_sock == INVALID_SOCKET)
	{
		err = WSAGetLastError();
		printf("socket error:%d\n", err);
		closesocket(conn_sock);
		WSACleanup();
		return -1;
	}
	bool bOpt = true;
	//打开广播选项
	setsockopt(conn_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));

	// 设置发往的地址
	SOCKADDR_IN saUdpServ;
	memset(&saUdpServ, 0, sizeof(saUdpServ));
	saUdpServ.sin_family = AF_INET;
	saUdpServ.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
	saUdpServ.sin_port = htons(7001);
	int nSize = sizeof(SOCKADDR_IN);

	char buff[MAX_BUF_LEN];
	memset(buff, 0, MAX_BUF_LEN);
	strcpy(buff, (char*)IpParameter);
	int nLoop = 0;
	char* ip = NULL;
	int nSendSize = sendto(conn_sock, buff, strlen(buff), 0, (SOCKADDR*)&saUdpServ, nSize);
	if (nSendSize == SOCKET_ERROR)
	{
		err = WSAGetLastError();
		printf("sendto error:%d\n", err);
		closesocket(conn_sock);
		WSACleanup();
		return -1;
	}
	printf("send: %s\n", buff);
	Sleep(5000);
	closesocket(conn_sock);
	WSACleanup();
	return 0;
}
	

/*int main() {

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = inet_addr("192.168.1.3");//本机wlan网卡地址
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//循环接收数据
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];//缓冲区
	while (true)
	{
		printf("等待连接...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

		//接收数据
		int ret = recv(sClient, revData, 255, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		//发送数据
		const char * sendData = "你好，TCP客户端！\n";
		send(sClient, sendData, strlen(sendData), 0);
		closesocket(sClient);
	}

	closesocket(slisten);
	WSACleanup();
	return 0;
}
*/
#pragma endregion


