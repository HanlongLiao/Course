//
// Created by Liao on 2018/10/9.
//

#include <iostream>
#include <Ws2tcpip.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET server;
char sendBuf[1024];
HANDLE SendEvent;

DWORD WINAPI SockSend(LPVOID lpParameter);
DWORD WINAPI SockRecv(LPVOID lpParameter);

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

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (server == INVALID_SOCKET) {
		cout << "Error at socket() " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	//Bind server
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);

	iResult = bind(server, (SOCKADDR *)&service, sizeof(service));
	if (iResult == SOCKET_ERROR) {
		cout << "Error at bind() " << WSAGetLastError() << endl;
		iResult = closesocket(server);
		if (iResult == SOCKET_ERROR) {
			cout << "Error at closesocket() " << WSAGetLastError() << endl;
		}
		WSACleanup();
		return 1;
	}

	//listen for incoming connection requests
	// on the created socket
	if (listen(server, 1) == SOCKET_ERROR) {
		cout << "Error at listen() " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	cout << "Listening on socket..." << endl;

	//Create a socket for accepting incoming requests
	SOCKET AcceptSocket;
	cout << "Waiting for client to connect..." << endl;

	//Accept the connection
	while (true) {
		AcceptSocket = SOCKET_ERROR;
		while (AcceptSocket == SOCKET_ERROR) {
			AcceptSocket = accept(server, NULL, NULL);
		}
		cout << "Client connected!" << endl;
		break;
	}
	server = AcceptSocket;

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
			sendSize = send(server, sendBuf, strlen(sendBuf), 0);
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
		recvSize = recv(server, recvBuf, 128, 0);
		cout << "ok" << endl;
		cout << recvBuf << endl;
		memset(recvBuf, 0, recvSize);
	}
}