// clientTerminal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define BUFSIZE 4096
#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "27015"

#define EXIT_WORD "exit\n"


int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory(recvbuf, DEFAULT_BUFLEN);

	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);  //localhost ip address 127.0.0.1
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Redirect input
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE) return 1;
	CHAR chBuf[BUFSIZE];
	ZeroMemory(&chBuf, BUFSIZE);
	DWORD dwRead;
	BOOL bSuccess;

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		//printf("Bytes received: %d\n", iResult);
		printf("%s", recvbuf);
	}
	else if (iResult == 0)
		printf("Connection closed\n");
	else
		printf("recv failed with error: %d\n", WSAGetLastError());

	for (;;) {
		ZeroMemory(&chBuf, BUFSIZE);
		bSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
		chBuf[strlen(chBuf) - 2] = chBuf[strlen(chBuf) - 1];
		chBuf[strlen(chBuf) - 1] = 0;
		if (strcmp(chBuf, EXIT_WORD) == 0) {
			printf("goodbye...\n");
			break;
		}

		iResult = send(ConnectSocket, chBuf, (int)strlen(chBuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		//printf("Bytes Sent: %ld\n", iResult);
		//do {
		ZeroMemory(recvbuf, DEFAULT_BUFLEN);
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				//printf("Bytes received: %d\n", iResult);
				printf("%s", recvbuf);
			}
			else if (iResult == 0)
				printf("Connection closed\n");
			else
				printf("recv failed with error: %d\n", WSAGetLastError());

		//} while (iResult > 0);
	}

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
