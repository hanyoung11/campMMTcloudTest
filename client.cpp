#define _CRT_SECURE_NO_WARNINGS 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <iostream>

#pragma comment (lib, "ws2_32.lib")
using namespace std;

int main() {
	printf("\t\t\t< Client >\n");
	WSADATA wsdata;
	WSAStartup(MAKEWORD(2, 2), &wsdata);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 서버 ip주소 (테스트시는 루프백-127.0.0.1로)
	addr.sin_port = htons(8000);


	char msg[30] = "hello";
	int addrSz = sizeof(addr);
	sendto(sock, msg, sizeof(msg), 0, (SOCKADDR*)&addr, sizeof(addr));
	//recvfrom(sock, msg, sizeof(msg), 0, (SOCKADDR*)&addr, &addrSz);
	//cout << inet_ntoa(addr.sin_addr) << " >> " << msg << endl;
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;

}