#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

static void bail(char * on_what)
{
    //fputs(strerror(errno),stderr);
    //fputs(":",stderr);
//    fputs(on_what,stderr);
//    fputc("\n",stderr);

    perror(on_what);
    exit(1);
}


int main(int argc,char **argv)
{
    int z;
    char * srvr_addr = NULL;
    struct sockaddr_in adr_srvr;
    int len_inet;
    int s;
    struct servent *sp;
    char dtbuf[128];

    if (argc>=2)
    {
        srvr_addr = argv[1];
    }
    else srvr_addr = "127.0.0.1";

    sp = getservbyname("daytime","tcp");
    
    //get service port
    if(!sp)
    {
        fputs("Unkown service: datime tcp\n",stderr);
        exit(1);
    }
    //set server addr
    memset(&adr_srvr,0,sizeof adr_srvr);
    adr_srvr.sin_family = AF_INET;
    adr_srvr.sin_port = sp->s_port;
    if(!inet_aton(srvr_addr,&adr_srvr.sin_addr))
    {
        printf("Wrong address!\n");
    }
    len_inet = sizeof adr_srvr;

    //set tcp/ip socket
    s = socket(PF_INET,SOCK_STREAM,0);
    if (s==-1)
    bail("socket()");
    // connet to server
    z = connect(s,(struct sockaddr*)&adr_srvr,len_inet);
    if(z==-1)
    bail("connect()");

    z = read(s,&dtbuf,sizeof dtbuf -1);
    if (z==-1)
    {
        bail("read()");
    }
    dtbuf[z] = 0;
    printf("Date & Time is %s\n",dtbuf);
    close(s);
    putchar('\n');
    return 0;
}
