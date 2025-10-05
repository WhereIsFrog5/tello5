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
            if(len < sizeof(outBuf)-1) {
                /*
                something weird that tello does is that on responses like error or ok it does not
                add a newline while on responses on commands like baro? attitude? speed? battery?
                it adds a newline, so here we basically are checking if it added a newline
                if it did add a newline we just print it, if not we add a newline and also print
                for example, if we just do
                std::cout << "[ TELLO RESPONSE ]: " << outBuf << std::endl;
                without this if then it will look something like this:
                command
                [ TELLO RESPONSE ]: ok
                takeoff
                [ TELLO RESPONSE ]: ok
                land
                [ TELLO RESPONSE ]: ok
                battery?
                [ TELLO RESPONSE ]: 42

                attitude?
                [ TELLO RESPONSE ]: pitch:2;roll:0;yaw:76;

                baro?
                [ TELLO RESPONSE ]: 77.771301

                see those newlines anyways enough yapping
                */
                if(outBuf[len-1] != '\n'){
                    outBuf[len] = '\n';
                    outBuf[len+1] = '\0';
                }
                
            }
            std::cout << "[ TELLO RESPONSE ]: " << outBuf;
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
    Tello::sendCustomMessage("command");
  
    while(true) {
        char outBuf[1024] = {0};
        while(true) {
            int len = Tello::receiveState(outBuf, sizeof(outBuf));
            if (len > 0) {
                std::cout << "[ TELLO STATE ]: " << outBuf; // tello adds a newline on state
            }
        }
    }

    Tello::cleanup();
    return 0;
}
```

Tello SDK 1.3 can be found here: https://dl-cdn.ryzerobotics.com/downloads/tello/20180910/Tello%20SDK%20Documentation%20EN_1.3.pdf
