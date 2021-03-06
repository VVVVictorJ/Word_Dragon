// DragonClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#define NELEM(arr)      (sizeof(arr) / sizeof(arr[0]))
#pragma region Parameter
//-------------------------------------------------------------------------------//
#define successfulregistered 1001
#define someonehasregistered 1010
#define outofrange			 -1	
#define	usernamepasfault	 5
#define	successfullogin		 1002
#define success				 1111
#define urworry				 1000
#define information			 1011
//-------------------------------------------------------------------------------//

#pragma endregion

#pragma region Global
SOCKET clientSocket;
SOCKET broadSocket;
SOCKADDR_IN clientAddr;

#pragma endregion

#pragma region function

#pragma region screen
void choicescreen()
{
	std::cout << '\t' << "1.注册" << std::endl;
	std::cout << '\t' << "2.登陆" << std::endl;
}

void choicescreen2()
{
	std::cout << '\t' << "1.注册" << std::endl;
	std::cout << '\t' << "2.登陆" << std::endl;
	std::cout << '\t' << "3.游戏" << std::endl;
}
#pragma endregion

#pragma region BroadThread
DWORD WINAPI BroadThread(LPVOID IpParameter)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return -1;
	}

	SOCKET clientSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == clientSock)
	{
		err = WSAGetLastError();
		printf("socket error:%d\n", err);
		WSACleanup();
		return -1;
	}
	//创建本地地址信息并绑定
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(7001);
	if (bind(clientSock, (SOCKADDR*)&sin, sizeof(sin)) != 0)
	{
		err = WSAGetLastError();
		printf("bind error:%d\n", err);
		closesocket(clientSock);
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(clientAddr);
	int nSize;
	char recvBuff[1024];
	memset(recvBuff, 0, 1024);

	while (1) {
		nSize = recvfrom(clientSock, recvBuff, 1024, 0, (SOCKADDR*)&clientAddr, &addrLen);
		if (nSize == SOCKET_ERROR)
		{
			err = WSAGetLastError();
			printf("recv error:%d\n", err);
			break;
		}
		recvBuff[nSize] = '\0';
		printf("%s\n", recvBuff);

	}

	closesocket(clientSock);
	WSACleanup();
	return 0;
}

#pragma endregion

#pragma region RecvThread
DWORD WINAPI RecvMessageThread(LPVOID IpParameter)
{
	SOCKET client = (SOCKET)(LPVOID)IpParameter;
	myprotocl mp;
	while (true)
	{
		//std::cout << "启动线程" << std::endl;
		recv(client, (char*)&mp, sizeof(myprotocl), 0);
		switch (mp.s.status)
		{
		case success:
			std::cout << mp.parse.parse << std::endl;
			break;
		case urworry:
			std::cout << mp.parse.parse << std::endl;
			break;
		case information:
			std::cout << std::endl;
			std::cout << mp.parse.parse << std::endl;
		default:
			break;
		}
	}
	return true;
}
#pragma endregion

#pragma region Login
void login(SOCKET socket)
{

	myprotocl mp;
	memset(&mp, 0, sizeof(mp));
	myprotocl tmp;
	memset(&tmp, 0, sizeof(tmp));

	mp.s.status = 2;

	std::cout << "please input your id:";
	std::cin >> mp.login.username;
	std::cout << "please input password:";
	std::cin >> mp.login.password;
	//char send_buf[1024];
	//char recv_buf[1024];
	//memset(send_buf, 0, 1024);
	//memset(recv_buf, 0, 1024);
	//memcpy(send_buf, &msg, sizeof(send_buf));

	send(clientSocket, (char*)&mp, sizeof(myprotocl), 0);

	recv(clientSocket, (char*)&tmp, sizeof(myprotocl), 0);
	HANDLE recv = CreateThread(NULL, 0, RecvMessageThread, (void *)socket, 0, NULL);

	switch (tmp.s.status)
	{
	case usernamepasfault:
		std::cout << "username or password fault" << std::endl;
		Sleep(500);
		system("cls");
		break;
	case successfullogin:
		system("cls");
		std::cout << "log in successfully" << std::endl;
		Sleep(1000);

		//std::cout << "启动recv线程" << std::endl;
		if (NULL == recv)
		{
			exit(1);
		}
		int choice;
		memset(&tmp, 0, sizeof(tmp));
		char pars[20];
		while (true)
		{
			choicescreen2();
			std::cout << "input your choice:";
			std::cin >> choice;
			while ('\n' == getchar());
			switch (choice)
			{
			case 1:
				continue;
			case 2:
				continue;
			case 3:
				tmp.s.status = choice;
				std::cout << "input your parse: ";
				scanf_s("%s", &pars, 20);
				strcpy_s(tmp.parse.parse,strlen(pars)+1, pars);
				send(clientSocket, (char*)&tmp, sizeof(myprotocl), 0);
				//fflush(stdin);
				Sleep(2000);
				break;
			default:
				break;
			}
		}

	}

}
#pragma endregion

#pragma region Register
//注册
bool reg(SOCKET socket)
{
	myprotocl mp;
	memset(&mp, 0, sizeof(mp));
	while (true)
	{
		mp.s.status = 1;
		char recv_buf[1024];
		memset(recv_buf, 0, 1024);
		myprotocl tmp;
		memset(&tmp, 0, sizeof(tmp));

		std::cout << "please enter username：";
		std::cin.ignore();
		std::cin >> mp.reg.username;
		std::cout << "please enter password：";
		std::cin >> mp.reg.password;

		send(clientSocket, (char*)&mp, sizeof(myprotocl), 0);//发送

		int r = recv(clientSocket, (char*)&tmp, sizeof(myprotocl), 0);//接受反馈
		std::cout << tmp.s.status << std::endl;
		if (r > 0)
		{
			std::cout << "1" << std::endl;
			switch (tmp.s.status)
			{
			case successfulregistered:
				std::cout << "successful registered" << std::endl;
				break;
			case someonehasregistered:
				std::cout << "someone had registered." << std::endl;
				break;
			case outofrange:
				std::cout << "out of range." << std::endl;
				break;
			default:
				break;
			}
			break;
		}
	}
	Sleep(10);
	return true;
}

#pragma endregion

#pragma region init
bool init()
{
	WSAData wsadata;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

	if (NO_ERROR != iResult)
	{
		std::cout << "Error at WSAStartup()" << std::endl;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == clientSocket)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	unsigned long ul = 1;

	if (NO_ERROR != iResult)
	{
		printf("ioctlsocket failed with error: %ld\n", iResult);
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	while (true)
	{
		int ret = connect(clientSocket, (sockaddr*)&addr, sizeof(SOCKADDR));
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK || r == WSAEINVAL)
			{
				std::cout << r << std::endl;
				Sleep(20);
				continue;
			}
			else if (r == WSAEISCONN)//套接字原来已经连接！！
			{
				break;
			}
			else
			{
				std::cout << r << std::endl;
				std::cout << "发生错误" << std::endl;
				return false;
			}
		}
		if (ret == 0)
		{
			break;
		}
	}

	return TRUE;
}

#pragma endregion

#pragma region Accept
void acpt(SOCKET socketed)
{
	int choice = 0;
	while (true)
	{
		std::cout << std::endl;
		std::cout << '\t' << "please choice the options" << std::endl;
		choicescreen();
		std::cout << std::endl;
		std::cin >> choice;
		while ('\n' == getchar());
		switch (choice)
		{
		case 1:
			//注册
			reg(socketed);
			std::cout << '\t' << "successful registered~" << std::endl;
			break;
		case 2:
			//登陆
			login(socketed);
			break;
		default:
			break;
		}
		system("cls");
	}
}

#pragma endregion

#pragma endregion
//-------------------------------------------------------------------------------//
int main()
{
	init();
	HANDLE broa = CreateThread(NULL, 0, BroadThread, NULL, 0, NULL);
	acpt(clientSocket);

	return std::cin.get();
}
