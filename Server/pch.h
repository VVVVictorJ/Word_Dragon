// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H
#include<tchar.h>
#include <stdio.h>
#include <winsock2.h>
#include<iostream>
#include<string>
#include<sstream>
#include<Windows.h>
#include<time.h>
#include<vector>
#include<list>
#include<iterator>
#include<algorithm>
#include<ctime>
#include<hash_map>
#include"CClient.h"
#pragma comment (lib, "ws2_32.lib")

#define successfulregistered 1001
#define someonehasregistered 1010
#define outofrange			 -1	
#define	usernamepasfault	 5
#define	successfullogin		 1002
#define success				 1111
#define urworry				 1000

// TODO: 添加要在此处预编译的标头

#endif //PCH_H
