#include "head.h"

#define SRVR_ADDRESS "127.0.0.1"
#define SRVR_PORT "9090"
#define MY_NAME "STUDENT"
#define MAX_CLIENT 64
#define MAX_MSG 120

#define CONN 0
#define DISCONN 1
#define TIME 2
#define NAME 3
#define LIST 4
#define SEND 5
#define QUIT 6


int s = -1;
//int c = -1;
//int z = -1;
struct sockaddr_in addr_srvr,addr_clnt;
char *srvr_addr = NULL;
char *srvr_port = NULL;
int len_inet;

typedef struct 
{
    int client_id;
    struct sockaddr_in addr;
}ClientInfo;

ClientInfo client[MAX_CLIENT];

void bail(char * on_what)
{
    perror(on_what);
}


void code_message(char * msg_send,char * msg_type,char*ip,char*port,char*id,char*message)
{
    char *p = msg_send;
    memset(p,' ',120);
    //copy type
    strcpy(p,msg_type);
    p[strlen(msg_type)] = ' ';
    p = p+4;
    //copy id
    strcpy(p,ip);
    p[strlen(ip)] = ' ';
    p = p+15;
    //copy port
    strcpy(p,port);
    p[strlen(port)] = ' ';
    p = p+5;
    //copy id
    strcpy(p,id);
    p[strlen(id)] = ' ';
    p = p+2;
    //copy message
    strcpy(p,message);
    p[strlen(message)] = ' ';
    p[120] = '\0';
}

int get_id(char *msg)
{
    return (msg[4]-'0')*10+(msg[5]-'0');
}

void code_from(char * from,char *seq,char * ip,char * port,char* id)
{
    char * p=from;
    memset(p,' ',94);
    //add seq
    strcpy(p,seq);
    p[strlen(seq)]=' ';
    p = p+2;
    //add ip
    strcpy(p,ip);
    p[strlen(ip)] = ' ';//remove '\0'
    p = p + 15;
    strcpy(p,port);
    p[strlen(port)] = ' ';//remove '\0'
    p = p+5;
    strcpy(p,id);
    p[94] = '\0';
}

void* run(void *id)
{
    int client_id = *((int *)id);
    char msg_recv[MAX_MSG+1];
    char msg_send[MAX_MSG+1];
    const char com[][7] = {{" "},{"DISC00"},{"TIME00"},{"NAME00"},{"LIST00"},{"MSG"}};
    int z=-1;
    int i;
    char send_id;
    time_t t;
    char TYPE[5];
    char IP[16];
    char PORT[6];
    char ID[3];
    char MSG[95];
//start server circle
    while(1)
    {
        if((z = recv(client_id,msg_recv,6,0))<=0)continue;
        msg_recv[z] = '\0';
  //      printf("server receive %s\n",msg_recv);
        for(i=0;i<6;i++)
        {
            if(!strcmp(msg_recv,com[i]))break;
        }
        switch(i)
        {
           case TIME:              
               {

                   time(&t);
                   strcpy(TYPE,"TIME");
                   sprintf(PORT,"%d",atoi(SRVR_PORT));
                   sprintf(ID,"%d",s);
                   strcpy(IP,SRVR_ADDRESS);
                   z = (int)strftime(MSG,94,"%A %b %d %H:%M:%S %Y ",localtime(&t));
                   //if(z<=0)
                   //{
                   //    error()//handle error
                  // }
                   code_message(msg_send,TYPE,IP,PORT,ID,MSG);
                   //printf("message send is %s\n",msg_send);
                   send(client_id,msg_send,strlen(msg_send),0);
               }
               break;
           case NAME:
               {
                   strcpy(TYPE,"NAME");
                   sprintf(PORT,"%d",atoi(SRVR_PORT));
                   strcpy(IP,SRVR_ADDRESS);
                   sprintf(ID,"%d",s);
                   strcpy(MSG,MY_NAME);
                   code_message(msg_send,TYPE,IP,PORT,ID,MSG);
                   send(client_id,msg_send,strlen(msg_send),0);
               }
               break;
           case LIST:
               {
                   strcpy(TYPE,"LIST");
                   int sequen = 1;
                   int tmp;
                   char seq[3]={"00"};
                   seq[2] ='\0';
                   for(i=0;i<MAX_CLIENT;i++)
                   {
     //                  printf("now is detect no %d\n",i);
                       if(client[i].client_id!=-1)
                       {
        //                   printf("client id %d\n",i);
                           
                           tmp = sequen++;
                           strcpy(seq,"00");
                           char *tmp_p = seq+1;
                      //     printf("tmp is %d\n",tmp);
                           while(tmp)
                           {
                               *tmp_p = tmp%10+'0';
                               tmp/=10;
                               tmp_p--;
                           }
                    //       printf("seq is %s",seq);
                           //client exist then send client package
                           strcpy(IP,inet_ntoa(((client[i].addr).sin_addr)));
                           sprintf(ID,"%d",client[i].client_id);
                           sprintf(PORT,"%d",(unsigned int)ntohs(client[i].addr.sin_port));
                           code_from(MSG,seq,IP,PORT,ID);
     //                      printf("MSG is %s\n",MSG);
                           //insert sequence to messge here
                           code_message(msg_send,TYPE,IP,PORT,ID,MSG);
   //                        printf("msg_seng is %s\n",msg_send);
                           send(client_id,msg_send,strlen(msg_send),0);
                       }
                   }
                   strcpy(seq,"-1");
                   code_message(msg_send,TYPE,IP,PORT,ID,seq);
//                   printf("lst message is :%s\n",msg_send);
                   send(client_id,msg_send,strlen(msg_send),0);
               }
               break;
           case DISCONN:
               {
                   client[client_id].client_id = -1;
                   pthread_exit(NULL);
               }
               break;
           default://send message
               {
//                   printf("Client is going to send message!\n");
                   strcpy(TYPE,"MESG");
                   send_id = get_id(msg_recv);
//                   printf("send id is: %d",send_id);
                   if(client[send_id].client_id==-1)
                   {
                       continue;
                   }                  
                 
                   while((z = recv(client_id,msg_recv,90,0))==-1);
                   msg_recv[z] = '\0';
    //               printf("Message going to send is %s\n,len is %d",msg_recv,(int)strlen(msg_recv));
                   strcpy(IP,inet_ntoa(((client[client_id].addr).sin_addr)));
                   sprintf(PORT,"%d",ntohs(client[client_id].addr.sin_port));
                   sprintf(ID,"%d",client_id);
    //               printf("TYPE %s IP%s ID %s PORT %s MSG %s \n",TYPE,IP,ID,PORT,msg_recv);
                   code_message(msg_send,TYPE,IP,PORT,ID,msg_recv);
  //                 printf("send message is %s,length is %d\n",msg_send,(int)strlen(msg_send));
                   send(send_id,msg_send,MAX_MSG,0);
               }
        }
    }
    exit(0);
}

int main(int argc,char **argv)
{
    int i;
    int c;
    int z;
    if(argc>=2)
    {
        srvr_addr = argv[1];
    }
    else
    {
        srvr_addr = SRVR_ADDRESS;
    }
    
    if(argc>=3)
    {
        srvr_port = argv[2];
    }
    else
    {
        srvr_port = SRVR_PORT;
    }

    s = socket(PF_INET,SOCK_STREAM,0);

    //init client
    for(i = 0;i<MAX_CLIENT;i++)
    {
        client[i].client_id = -1;
    }

   if(s==-1)
   {
       bail("socket()");
   }
    memset(&addr_srvr,0,sizeof addr_srvr);
    addr_srvr.sin_family = AF_INET;
    if(!inet_aton(srvr_addr,&addr_srvr.sin_addr))
    {
        bail("wrong addr");
    }
    addr_srvr.sin_port = htons(atoi(srvr_port));
    len_inet = sizeof addr_srvr;
    //bind
    z = bind(s,(struct sockaddr*)&addr_srvr,len_inet);
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
    while(1)
    {
        len_inet = sizeof addr_clnt;
        c = accept(s,(struct sockaddr*)&addr_clnt,&len_inet);
        if(c==-1)
        {
            continue;
        }
        else if(c>MAX_CLIENT)
        {
            continue;
        }
        printf("now connect %d\n",c);
        client[c].addr = addr_clnt;
        client[c].client_id = c;
        pthread_t pid;
        pthread_create(&pid,NULL,run,(void *)&c);
    }
}
