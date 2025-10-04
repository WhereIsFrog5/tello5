tello5 is windows-only C++ library for interacting with DJI Ryze Tello drone. It is focused on Tello SDK 1.3.

simple example, drone takes off, flips forward and then lands:
```cpp
#define TELLO_HPP_DEBUG
#include <tello.hpp>

int main() {
  if(!Tello::init()) {
    return -1;
  }

  Tello::sendCustomMessage("command");
  Tello::sendCustomMessage("takeoff");
  Tello::sendCustomMessage("flip f");
  Tello::sendCustomMessage("land");

  Tello::cleanup();
  return 0;
}
```

another example that emulates cmd to send commands and receive responses from the drone,
note that you have to send "command" first (without "") for it to work:
```cpp
#define TELLO_HPP_DEBUG
#include <tello.hpp>
#include <iostream>
#include <thread>

void receiveresponse() {
    char outBuf[1024] = {0};
    while(true) {
        int len = Tello::receiveResponse(outBuf, sizeof(outBuf));
        if (len > 0) {
            std::cout << "[ TELLO RESPONSE ]: " << outBuf << std::endl;
        }
    }
}

int main() {
    if(!Tello::init()) {
        return -1;
    }
    std::thread(receiveresponse).detach();
    std::string msg;
    while(true) {
        std::getline(std::cin, msg);
        if(msg == "e") {
            Tello::sendCustomMessage("emergency");
            continue;
        }
        Tello::sendCustomMessage(msg);
    }
    Tello::cleanup();
    return 0;
}
```
yet another example to get tello state:
```cpp
#define TELLO_HPP_DEBUG
#include <tello.hpp>
#include <iostream>

int main() {
    if(!Tello::init()) {
        return -1;
    }

    // note that you will need to send "command" to be able to use sdk commands
    // it will look something like this
    // Tello::sendCustomMessage("command");
  
    while(true) {
        char outBuf[1024] = {0};
        while(true) {
            int len = Tello::receiveState(outBuf, sizeof(outBuf));
            if (len > 0) {
                std::cout << "[ TELLO STATE ]: " << outBuf;
            }
        }
    }

    Tello::cleanup();
    return 0;
}
```
