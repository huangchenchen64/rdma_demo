#ifndef CLIENT
#define CLIENT

#include <thread>
#include "RdmaSocket.hpp"
#include "Configuration.hpp"
#include "MemoryManager.h"
#include "global.h"

using namespace std;
class Client {
private:
    Configuration *conf;
    RdmaSocket *socket;
    MemoryManager *mem;
    bool isServer;
    uint32_t taskID;
public:
    uint64_t mm;
    Client(Configuration *conf, RdmaSocket *socket, MemoryManager *mem, uint64_t mm);
    Client();
    ~Client();
    RdmaSocket* getRdmaSocketInstance();
    Configuration* getConfInstance();
    bool RdmaCall(uint16_t DesNodeID, char *bufferSend, uint64_t lengthSend, char *bufferReceive, uint64_t lengthReceive);
    uint64_t ContractSendBuffer(GeneralSendBuffer *send);
};

#endif // CLIENT

