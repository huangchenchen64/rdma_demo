#include "MemoryManager.h"

MemoryManager::MemoryManager(uint64_t _mm, uint64_t _ServerCount,  int _DataSize)
: ServerCount(_ServerCount), MemoryBaseAddress(_mm) {
    void *shmptr;
    if (_mm == 0) {
        /* Open Shared Memory. */
        /* Add Data Storage. */
        DMFSTotalSize = (uint64_t)_DataSize;
        DMFSTotalSize = DMFSTotalSize * 1024 * 1024 * 1024;
        //DMFSTotalSize = DMFSTotalSize * 1024; //deleted by weixing
        /* Add Metadata Storage. */
        DMFSTotalSize += METADATA_SIZE;
        /* Add Client Message Pool. */
        DMFSTotalSize += CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
        /* Add Server Message Pool. */
        DMFSTotalSize += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * ServerCount;
        Debug::notifyError("total_size=%lu",DMFSTotalSize + LOCALLOGSIZE + DISTRIBUTEDLOGSIZE);
        shmid = shmget(SHARE_MEMORY_KEY, DMFSTotalSize + LOCALLOGSIZE + DISTRIBUTEDLOGSIZE, IPC_CREAT);
        //shmid = shmget(SHARE_MEMORY_KEY,2048, IPC_CREAT);
        if (shmid == -1) {
            Debug::notifyError("21::shmget error");
        }
        shmptr = shmat(shmid, 0, 0);
        Debug::notifyError("shmptr = %p",shmptr);
        if (shmptr == (void *)(-1)) {
            Debug::notifyError("25::shmat error");
        }
        MemoryBaseAddress = (uint64_t)shmptr;
        memset((void *)MemoryBaseAddress, '\0', DMFSTotalSize + LOCALLOGSIZE + DISTRIBUTEDLOGSIZE);

        //memset((void *)MemoryBaseAddress, '\0', 1024);
    }
    ClientBaseAddress = MemoryBaseAddress;
    ServerSendBaseAddress = MemoryBaseAddress + CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
    ServerRecvBaseAddress = ServerSendBaseAddress + SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * ServerCount;
    MetadataBaseAddress = ServerRecvBaseAddress + SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * ServerCount;
    DataBaseAddress = ServerRecvBaseAddress + METADATA_SIZE;
    memset((void *)DataBaseAddress, 'a', _DataSize*1024);
    // add by weixing [20190329]:b
    FreeMemoryPointer = (BlockHeader*)DataBaseAddress;
    FreeMemoryPointer->info.addrBlock_ = (char*)DataBaseAddress + sizeof(BlockHeader);
    FreeMemoryPointer->info.size_ = DMFSTotalSize * 1024 * 1024 * 1024 - sizeof(BlockHeader);
    // add e
    //deleted by weixing [20190329]:b
//    LocalLogAddress = _DataSize;
//    LocalLogAddress *= (1024);
//    LocalLogAddress += DataBaseAddress;
//    Debug::debugItem("LocalLogAddress = %lx", LocalLogAddress);
//    DistributedLogAddress = LocalLogAddress + LOCALLOGSIZE;
    //delete e
    SendPoolPointer = (uint8_t *)malloc(sizeof(uint8_t) * ServerCount);
    memset((void *)SendPoolPointer, '\0', sizeof(uint8_t) * ServerCount);
}

MemoryManager::~MemoryManager() {
    Debug::notifyInfo("Stop MemoryManager.");
    free(SendPoolPointer);
    shmctl(shmid, IPC_RMID , 0);
    Debug::notifyInfo("MemoryManager is closed successfully.");
}

uint64_t MemoryManager::getDmfsBaseAddress() {
    return MemoryBaseAddress;
}

uint64_t MemoryManager::getDmfsTotalSize() {
    return DMFSTotalSize;
}

uint64_t MemoryManager::getMetadataBaseAddress() {
    return MetadataBaseAddress;
}

uint64_t MemoryManager::getDataAddress() {
    return DataBaseAddress;
}

uint64_t MemoryManager::getServerSendAddress(uint16_t NodeID, uint64_t *buffer) {
    // uint8_t temp = __sync_fetch_and_add( &SendPoolPointer[NodeID - 1], 1 );
    // temp = temp % SERVER_MASSAGE_NUM;
    uint32_t tid = gettid();
    uint64_t offset = (uint64_t)th2id[tid];
    *buffer = (ServerSendBaseAddress +
        (NodeID - 1) * SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM
        + offset * SERVER_MASSAGE_SIZE);
    return offset;
}

uint64_t MemoryManager::getServerRecvAddress(uint16_t NodeID, uint16_t offset) {
    uint64_t buffer;
    buffer = (ServerRecvBaseAddress +
        (NodeID - 1) * SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM
        + offset * SERVER_MASSAGE_SIZE);
    return buffer;
}

uint64_t MemoryManager::getClientMessageAddress(uint16_t NodeID) {
    return ClientBaseAddress + (NodeID - ServerCount  - 1) * CLIENT_MESSAGE_SIZE;
}

uint64_t MemoryManager::getLocalLogAddress() {
    return LocalLogAddress;
}

uint64_t MemoryManager::getDistributedLogAddress() {
    return DistributedLogAddress;
}

void MemoryManager::setID(int ID) {
    uint32_t tid = gettid();
    th2id[tid] = ID;
}

// add by weixing [20190329]:b
int MemoryManager::allocateMemoryBlocks(uint64_t num, GAddr *addrList){
    if()
}
// add e




