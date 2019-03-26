
#include "client.h"


int main()
{

    Client *client = new Client();
    uint64_t DmfsDataOffset;
    DmfsDataOffset =  CLIENT_MESSAGE_SIZE * MAX_CLIENT_NUMBER;
    DmfsDataOffset += SERVER_MASSAGE_SIZE * SERVER_MASSAGE_NUM * client->getConfInstance()->getServerCount();
    DmfsDataOffset += METADATA_SIZE;

    char value[1024];
    memset(value,0,1024);
    uint64_t size=1024;
    uint64_t SendPoolAddr = client->mm + 4 * 1024;
    client->getRdmaSocketInstance()->RdmaRead(1,SendPoolAddr,0+DmfsDataOffset,size,0);


    printf("Read:%s\n",SendPoolAddr);

    for(int i=0;i<size;i++){
        value[i]='0'+i%10;
    }
    memcpy((void *)SendPoolAddr, (void *)(value), size);
    memset(value,0,1024);

    client->getRdmaSocketInstance()->RdmaWrite(1, SendPoolAddr,DmfsDataOffset, size,-1,0);
    printf("Write\n");

    uint64_t ReceivePoolAddr = client->mm + 6 * 1024;
    GeneralSendBuffer *send = (GeneralSendBuffer*)SendPoolAddr;
    send->message = MESSAGE_TEST;
    memcpy(send->path,"1234",5);
    client->RdmaCall(1, (char*)send, size, value, size);
    GeneralSendBuffer *receive = (GeneralSendBuffer*)value;
    printf("RdmaCall:%d,%s\n",receive->message,receive->path);



    client->getRdmaSocketInstance()->RdmaRead(1,SendPoolAddr,0+DmfsDataOffset,size,0);
    memcpy((void *)(value), (void *)SendPoolAddr, size);
    printf("Read:%s\n",value);
    return 0;
}
