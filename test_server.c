#include "head.h"
#define default_addr "127.0.0.1"

void bail(char * on_what)
{
    perror(on_what);
    exit(-1);
}

int main()
{
    struct sockaddr_in addr_srvr;
    int s;
    int z;
    char btbuf[128];
    int len_inet;
    int bufsize = sizeof btbuf;
//    printf("buf size is %d\n",bufsize);
    //socket word
    s = socket(PF_INET,SOCK_STREAM,0);
    if(s==-1)
    {
        bail("socket()");
    }
    else
    {
        printf("s is %d\n",s);
    }

    //set  address
    memset(&addr_srvr,0,sizeof addr_srvr);
    addr_srvr.sin_family = PF_INET;
    addr_srvr.sin_port = htons(9090);
    if(!inet_aton(default_addr,&addr_srvr.sin_addr))
    {
        bail("wrong addr!");
    }
    len_inet = sizeof addr_srvr;
    //connect to server
    z = connect(s,(struct sockaddr*)&addr_srvr,len_inet);
    if(z==-1)
    {
        bail("connect()");
    }
    else
    {
        printf("z is %d\n",z);
    }
    //write to serv
    while(1){
    strcpy(btbuf,"time");
    send(s,btbuf,sizeof(btbuf),0);
    z = recv(s,btbuf,2,0);
    if(z<0)
    {
        printf("receive nothing!\n");
    }
    else 
    {
        printf("%d\n",z);
        btbuf[z] = '\0';
    }
    printf("get first is %s",btbuf);
    z = recv(s,btbuf,10,0);
    btbuf[z] = '\0';
    printf("z is %d",z);
    printf("get from server is %s\n",btbuf);
    }
    close(s);
    return 0;
}
