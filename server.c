#include "head.h"
#include <time.h>
#include <signal.h>
#include <pthread.h>
#define default_addr "127.0.0.1"
#define default_port "9090"
#define  MAX_CLIENT 64
#define MAX_MSG 128
#define MAX_COM 5
#define DISCONN 0
#define TIME 1
#define NAME 2 
#define LIST 3
#define SEND 4
#define MY_NAME "STUDENT"
const char com[MAX_COM][10] = {{"disconn"},{"time   "},{"name   "},{"list   "},{"send   "}};
char btbuf[MAX_MSG];
fd_set rx_set;
fd_set wk_set;
int mx;
    int s=-1; //socket for server
    int z;
struct timeval tv;

typedef struct
{
   struct sockaddr_in addr;
    int client;
}ClientInfo;

ClientInfo client[MAX_CLIENT];


void add(int id)
{
    
}

void delete_client(int id)
{
}

void bail(char * on_what)
{
    perror(on_what);
    exit(-1);
}

void Disconn(int c)
{
    FD_CLR(c,&rx_set);
    delete_client(c);
}

void Name(int c)
{
    strcpy(btbuf,MY_NAME);
    send(c,btbuf,strlen(btbuf),0);
}

void List(int c)
{
    int cc;
    int port;
    char btbuf_p[25];
    char *p;
        for (cc=0;cc<mx;c++)
        {
        if(cc!=s)
        {
        if(FD_ISSET(cc,&rx_set))
        {
            memset(btbuf_p,'0',sizeof btbuf_p);
            btbuf_p[24]='\0';
            btbuf_p[21] = '\t';
            p = btbuf_p;
            // build message sent to client
            strcpy(btbuf_p,inet_ntoa(client[c].addr.sin_addr));
            btbuf_p[strlen(btbuf_p)] = '\t';
            p = btbuf_p+20;
            port = ntohs((client[cc].addr).sin_port);
            while(port)
            {
                *p = port%10+'0';
                p--;
                port/=10;
            }
            p = btbuf_p+23;
            port = cc;

            while(port)
            {
                *p = port%10 +'0';
                p--;
                port/=10;
            }
            printf("%s\n",btbuf_p);
            send(c,btbuf_p,strlen(btbuf_p),0);
         
        }
         }
    strcpy(btbuf,"end");
    send(c,btbuf,strlen(btbuf),0);
        }
}
void Send(int c)
{
    int id;
    char btbuf_p[MAX_MSG];
    z = recv(c,btbuf_p,2,0);
    btbuf_p[z] = '\0';
    id = atoi(btbuf_p);
    //recive message
    recv(c,btbuf_p,MAX_MSG,0);
    send(id,btbuf_p,strlen(btbuf),0);
}


void mytime(int c)
{
    int z;
    time_t t;
    time(&t);
    z = (int)strftime(btbuf,sizeof btbuf,"%A %b %d %H:%M:%S %Y\n",localtime(&t));
   send(c,btbuf,z,0);
}


int main(int argc,char **argv)
{
    char * srvr_addr = NULL; //addr of server
    char*  srvr_port = NULL;//port of server
    int c =-1;
    struct sockaddr_in addr_srvr; //AF_INET
    struct sockaddr_in addr_clnt;//AF_INET
    int len_int;// LEN OF ADDR
    int n; //return value from select
    int i;
    //init client set 
    //set addr and port of server
signal(SIGHUP,SIG_IGN);

signal(SIGINT,SIG_IGN);
signal(SIGPIPE,SIG_IGN);
    if(argc>=2)
    {
        srvr_addr = argv[1];
    }
    else 
    {
        srvr_addr = default_addr;
    }
    if(argc>=3)
    {
        srvr_port = argv[2];
    }
    else
    { 
        srvr_port = default_port;
    }
    //set socket
    
    s = socket(PF_INET,SOCK_STREAM,0);
    if(s==-1)
    {
        bail("socket()");    
    }
    //set socket addr of server
    
    memset(&addr_srvr,0,sizeof addr_srvr);
    addr_srvr.sin_family = AF_INET;
    if(!inet_aton(srvr_addr,&addr_srvr.sin_addr))
    {
        bail("wrong addr");
    }
    addr_srvr.sin_port = htons(atoi(srvr_port));
    len_int = sizeof addr_srvr;
    //bind
    z = bind(s,(struct sockaddr*)&addr_srvr,len_int);
    if(z==-1)
     {
         bail("bind()");
     }

     //listen
     z = listen(s,10);
     if(z==-1)
     {
         bail("listen()");
     }
    //add s into rx_set
     FD_ZERO(&rx_set);
     FD_SET(s,&rx_set);
     mx = s+1;
     //serving circle
     for(;;)
     {
         //init work_set
         FD_ZERO(&wk_set);
         //copy read set into workset
         for(z = 0;z<mx;z++)
         {
             if(FD_ISSET(z,&rx_set))
             {
                 FD_SET(z,&wk_set);
             }
         }

         tv.tv_sec = 2;
         tv.tv_usec = 30000;

         n = select(mx,&wk_set,NULL,NULL,&tv);

         if(n==-1)
         {
             bail("error occured while select");
         }
         else if(!n)
         {
             //time out
             continue;
         }

         if(FD_ISSET(s,&wk_set))//if s in wk_set ,then a request occur
         {

             len_int = sizeof addr_clnt;
             c = accept(s,(struct sockaddr*)&addr_clnt,&len_int);
           //  printf("now we get %d\n",c);
             if(c==-1)
                 bail("accept()");
             if(c>MAX_CLIENT)            
             {
                 
                 //over max client restrict
                 close(c);
                 continue;
             }
            //set file stream
             //more client get conneted
             if(c+1>mx)mx = c+1;
       //      printf("mx is %d,c is %d",mx,c);
       //     setlinebuf(client[c].rx);
         //   setlinebuf(client[c].wx);        
             client[c].addr = addr_clnt;
             client[c].client =c;
             add(c);
            FD_SET(c,&rx_set);
         }
         for(c=0;c<mx;c++)
          {
             if(FD_ISSET(c,&wk_set))
             {
                 if(c!=s)
                 {
                 z = recv(c,btbuf,7,0);
                 if(z==-1)continue;
                 btbuf[z] = '\0';
            //     printf("z is %d\n",z);
                 for(i = 0;i<MAX_COM;i++)
                 {
                     if(!strcmp(btbuf,com[i]))break;
                 }
            //     printf("%d\n",i);
                 switch(i)
                 {
                     case DISCONN: Disconn(c);break;
                     case NAME:Name(c);break;
                     case LIST:List(c);break;
                     case SEND:Send(c);break;
                     case TIME:mytime(c);break;
                     default:strcpy(btbuf,"error command!\n");send(c,btbuf,strlen(btbuf),0);//this line will never be used, just in case
                 }
                 }
             }
         }
         //decrease mx if possible
         for(c = mx-1;c>=0&&!FD_ISSET(c,&rx_set);c = mx-1)mx = c;
     }
     //it won't get here
     printf("exit here\n");
     return 0;
}
