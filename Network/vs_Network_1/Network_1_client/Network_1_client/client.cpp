#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
using namespace std;
#pragma	comment(lib, "ws2_32.lib")

DWORD WINAPI SockSend(LPVOID lpParameter);
DWORD WINAPI SockRecv(LPVOID lpParameter);

SOCKET client;
char sendBuf[1024];

HANDLE SendEvent;

int main() {
	//Init
	WSADATA wsadata;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);

	if (iResult != NO_ERROR) {
		cout << "Error at WSAStratup()" << endl;
		WSACleanup();
		return 1;
	}

	//Establish socket

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client == INVALID_SOCKET) {
		cout << "Error at socket() " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in addr_sev;
	addr_sev.sin_family = AF_INET;
	addr_sev.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr_sev.sin_port = htons(27015);

	//Connect to server
	if (SOCKET_ERROR == connect(client, (sockaddr *)&addr_sev, sizeof(addr_sev))) {
		cout << "Error at connect()" << endl;
		WSACleanup();
		return 1;
	}
	HANDLE rThread = CreateThread(NULL, 0, SockRecv, NULL, 0, NULL);
	//CloseHandle(rThread);

	HANDLE sThread = CreateThread(NULL, 0, SockSend, NULL, 0, NULL);
	//CloseHandle(sThread);

	SendEvent = CreateEvent(NULL, false, false, NULL);
	while (true) {
		cin >> sendBuf;
		SetEvent(SendEvent);
	}
	CloseHandle(SendEvent);

	WSACleanup();
	return 0;
}

DWORD WINAPI SockSend(LPVOID lpParameter) {
	DWORD sendSize;
	DWORD dw;
	memset(sendBuf, 0, 1024);
	while (true) {
		dw = WaitForSingleObject(SendEvent, INFINITE);
		switch (dw) {
		case WAIT_OBJECT_0:
			sendSize = send(client, sendBuf, strlen(sendBuf), 0);
			memset(sendBuf, 0, sendSize);
			break;
		case WAIT_TIMEOUT:
			cout << "Timeout" << endl;
			break;
		case WAIT_FAILED:
			cout << "Send massage failed" << endl;
			break;
		}
	}
}

DWORD WINAPI SockRecv(LPVOID lpParameter) {
	char recvBuf[1024];
	DWORD recvSize;
	memset(recvBuf, 0, 1024);
	while (true) {
		recvSize = recv(client, recvBuf, 128, 0);
		cout << "ok" << endl;
		cout << recvBuf << endl;
		memset(recvBuf, 0, recvSize);
	}
}