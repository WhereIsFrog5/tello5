#pragma once

#include <string>

namespace Tello {
    bool init();
    void cleanup();
    bool sendCustomMessage(std::string sendBuffer);
    int  receiveResponse(char* outBuf, int maxLen);
    int  receiveState(char* outBuf, int maxLen);
};
