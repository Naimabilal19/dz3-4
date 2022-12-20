#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "windows.h"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;


void CheckMess(char* Buf, int& seconds) {
    char hamburger[] = "hamburger";
    char cola[] = "cola";
    char potato[] = "potato";
    int size = strlen(Buf);

    int first = 0, i = 0, h = 0, c = 0, p = 0;
    int countH = 0, countC = 0, countP = 0;
    while (i < size) {
        while (Buf[i] != ' ') {
            if (Buf[i] == hamburger[h]) {
                countH++;
                h++;
            }
            if (Buf[i] == cola[c]) {
                countC++;
                c++;
            }
            if (Buf[i] == potato[p]) {
                countP++;
                p++;
            }
            i++;
        }
        if (countH == strlen(hamburger)) {
            seconds += 5000;
            countH = 0;
        }
        if (countC == strlen(cola)) {
            seconds += 3000;
            countC = 0;
        }
        if (countP == strlen(potato)) {
            seconds += 1000;
            countP = 0;
        }
        i++;
    }
}

int main()
{
    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failked with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
    {
        cout << "bind failed with error " << WSAGetLastError() << endl;
        return 3;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];

    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    cout << "Receiving data.." << endl;
    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "recvfrom failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';
    cout << "Client: " << receiveBuf << endl;
    int seconds = 0;
    CheckMess(receiveBuf, seconds);


    const size_t sendBufSize = 1024;
    char sendBuf[sendBufSize];
    string str = to_string(seconds);
    str += " milliseconds";
    strcpy(sendBuf, str.c_str());

    int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&senderAddr, senderAddrSize);
    if (sendResult == SOCKET_ERROR)
    {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    Sleep(seconds);
    char sendBuf2[] = "Your order is ready!";
    int sendResult2 = sendto(udpSocket, sendBuf2, strlen(sendBuf2), 0, (SOCKADDR*)&senderAddr, senderAddrSize);
    if (sendResult == SOCKET_ERROR) {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    closesocket(udpSocket);
    WSACleanup();
}



