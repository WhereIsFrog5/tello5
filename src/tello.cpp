#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <tello.hpp>
    #include <iostream>

    #define TELLO_RECEIVE 8889
    #define TELLO_STATE   8890
    #define TELLO_IP      "192.168.10.1"

    bool initialized = false;

    WSAData wsa;
    SOCKET sock;
    SOCKET sockState; /* used for receiving state */
    sockaddr_in server;
    sockaddr_in receive;
    sockaddr_in state;

    bool initSock(SOCKET& sock, sockaddr_in& addr) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sock == INVALID_SOCKET) {
            //WSACleanup();
            return false;
        }

        if(bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(sock);
            return false;
        }
        return true;
    }

    bool checkSock(SOCKET& sock) {
        return sock != INVALID_SOCKET;
    }
    
    bool Tello::init() {
        if(!initialized) {
            if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
            #ifdef TELLO_HPP_DEBUG
                std::cout << "WSAStartup failed!\n";
            #endif
                return false;
            }
            initialized = true;
        }

        receive.sin_family = AF_INET;
        receive.sin_port = htons(TELLO_RECEIVE);
        receive.sin_addr.s_addr = inet_addr("0.0.0.0");;
        if(!initSock(sock, receive)) {
        #ifdef TELLO_HPP_DEBUG
            std::cout << "Initializing sock receive failed!\n";
        #endif
            WSACleanup();
            initialized = false;
            //ExitProcess(0);
            return false;
        }

        state.sin_family = AF_INET;
        state.sin_port = htons(TELLO_STATE);
        state.sin_addr.s_addr = inet_addr("0.0.0.0");;
        if(!initSock(sockState, state)) {
        #ifdef TELLO_HPP_DEBUG
            std::cout << "Initializing sock state failed!\n";
        #endif
            WSACleanup();
            initialized = false;
            //ExitProcess(0);
            return false;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(TELLO_RECEIVE);
        server.sin_addr.s_addr = inet_addr(TELLO_IP);
        return true;
    }

    void Tello::cleanup() {
        if(checkSock(sock)) {closesocket(sock); sock = INVALID_SOCKET;}
        if(checkSock(sockState)) {closesocket(sockState); sockState = INVALID_SOCKET;}
        if(initialized) {
            WSACleanup();
            initialized = false;
        #ifdef TELLO_HPP_DEBUG
            std::cout << "cleanup finished!\n";
        #endif
        }
    }

    bool Tello::sendCustomMessage(std::string sendBuffer) {
        if(!checkSock(sock)) {
            return false;
        }
        return (sendto(sock, sendBuffer.c_str(), (int)sendBuffer.size(), 0, (sockaddr*)&server, sizeof(server)) != SOCKET_ERROR);
    }

    int Tello::receiveResponse(char* outBuf, int maxLen) {    
        if(!checkSock(sock)) {
        #ifdef TELLO_HPP_DEBUG
            std::cout << "invalid sock\n";
        #endif
            return -1;
        }
        if(maxLen <= 0) {
            return -1;
        }
        sockaddr_in from;
        int fromLen = sizeof(from);
        int recvLen = recvfrom(sock, outBuf, maxLen - 1, 0, (sockaddr*)&from, &fromLen);
        if(recvLen == SOCKET_ERROR) {
            return -1;
        }
        if(recvLen >= maxLen) {
            recvLen = maxLen - 1;
        }
        outBuf[recvLen] = '\0';
        return recvLen;
    }

    int Tello::receiveState(char* outBuf, int maxLen) {
        if(!checkSock(sockState)) {
        #ifdef TELLO_HPP_DEBUG
            std::cout << "invalid sock state\n";
        #endif
            return -1;
        }
        if(maxLen <= 0) {
            return -1;
        }
        sockaddr_in from;
        int fromLen = sizeof(from);
        int recvLen = recvfrom(sockState, outBuf, maxLen - 1, 0, (sockaddr*)&from, &fromLen);
        if(recvLen == SOCKET_ERROR) {
            return -1;
        }
        if(recvLen >= maxLen) {
            recvLen = maxLen - 1;
        }
        outBuf[recvLen] = '\0';
        return recvLen;
    }
#endif
