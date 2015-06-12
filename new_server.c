#include "head.h"

#define default_addr "127.0.0.1"
#define MAX_CLIENT 64
#define com 2
#define MY_NAME "STUDENT"
const char command[][10] = {{"time"},{"name"}};
    int s=-1;
    int c=-1;
    int z=-1;
    int i;
    int queue[MAX_CLIENT];
    int top = 0;
    int len_inet=0;
    char btbuf[128];
    int bufsize=0;
    time_t t;
    struct sockaddr_in addr_srvr,addr_clnt;
void mytime()
{
   time(&t);


   z = (int)strftime(btbuf,sizeof btbuf,"%A %b %d %H:%M:%S %Y\n",localtime(&t));
   send(c,btbuf,z,0);
}


void myname()
{
    strcpy(btbuf,MY_NAME);
    send(c,btbuf,strlen(btbuf),0);
}


int main()
{

    s = socket(PF_INET,SOCK_STREAM,0);
    if(s==-1)
    {
        printf("wrong s\n");
    }
    else
    {
        printf("server s is %d\n ",s);
    }

    //set addr
    memset(&addr_srvr,0,sizeof addr_srvr);
    addr_srvr.sin_family = PF_INET;
    addr_srvr.sin_port=htons(9090);
    if(!inet_aton(default_addr,&addr_srvr.sin_addr))
    {
        printf("wrong addr\n");
    }
    len_inet = sizeof(addr_srvr);

    z = bind(s,(struct sockaddr*)&addr_srvr,len_inet);
    if(z==-1)
    {
        printf("bind error\n");
    }

    //set as listen port
    z = listen(s,10);
    if(z==-1)
    {
        printf("listen error!\n");
    }

    while(1)
    {
        len_inet = sizeof addr_clnt;
        c = accept(s,(struct sockaddr*)&addr_clnt,&len_inet);
       // printf("thifd\n");
        if(c==-1)
        {
            printf("accpet error!\n");
        }
        else
        {
            printf("it's now accept %d\n",c);
        }
        int  pid = fork();
        if(pid<0)
        {
            printf("wrong create!\n");
        }
        else if(pid ==0)
        {
        z = recv(c,btbuf,10,0);
        btbuf[z] = '\0';
        for(i = 0;i<com;i++)
        {
            if(!strcmp(command[i],btbuf))break;
        }

        switch(i)
        {
            case 0:mytime();break;
            case 1:myname();break;
        }
//        strcpy(btbuf,"hello");
//        send(c,btbuf,strlen(btbuf),0);
        queue[top++] = c;
        exit(0);
        }
        else
        {
            printf("father here\n");
        }
    }
    return 0;
}

