#include "head.h"

//define some costant
#define MAX_MSG 120
#define MAX_COM 7

#define CONN 0
#define DISCONN 1
#define TIME 2
#define NAME 3
#define LIST 4
#define SEND 5
#define QUIT 6

const char com[][8] = {{"conn"},{"disconn"},{"time"},{"name"},{"list"},{"send"},{"quit"}};
const char com_s[][7] ={{" "},{"DISC00"},{"TIME00"},{"NAME00"},{"LIST00"},{"MESG00"}};

//function declaration
void * receive(void * id);  // receive message from server
void bail(char *on_what);

//int global = 1;

int main()
{
    int s = -1,*s_client;//socket of server
    struct sockaddr_in addr_srvr;//addr of server
    char msg_send[MAX_MSG+1];//message send to server
    char input[10];
    int i;
    int len_inet;
    int z;//error info
    char srvr_addr[20];//server address
    char srvr_port[10];//server port
    pthread_t pid;
    int father = 0;
    int send_id;
    printf("Welcome to Student client! Have fun!\n");

    //start serve circle
    while(1)
    {
        scanf("%s",input);
        for(i = 0;i<MAX_COM;i++)
        {
            if(!strcmp(input,com[i]))break;
        }
//        printf("%s %d \n",input,i);
//        printf("%d\n",i);
        if(i!=CONN&&i!=QUIT&&(s==-1)&&i<MAX_COM){printf("You are not connected!\n");continue;}
        if(i>=MAX_COM){printf("Wrong input!\n");continue;}
        switch(i)
        {
            case CONN: 
                {
                    if(s!=-1)
                    {
                        printf("You are now connected! please quit first!\n");continue;
                    }
                    scanf("%s",srvr_addr);
                    scanf("%s",srvr_port);                    
                    s = socket(PF_INET,SOCK_STREAM,0);//generate socket word
                    if(s==-1)
                    {
                        bail("socket()");
                        continue;
                    }

                    //set server address
                    memset(&addr_srvr,0,sizeof addr_srvr);
                    addr_srvr.sin_family = PF_INET;
                    addr_srvr.sin_port = htons(atoi(srvr_port));
                    if(!inet_aton(srvr_addr,&addr_srvr.sin_addr))
                    {
                        bail("wrong address");
                        s = -1;
                        continue;
                    }
                    len_inet = sizeof addr_srvr;

                    //connect
                    z = connect(s,(struct sockaddr*)&addr_srvr,len_inet);//this will triger a accept event in server
                    if(z==-1)
                    {
                        bail("connect()");
                        s =  -1;
                        continue;
                    }

                    //create thread to receive from server
                    s_client = (int *)malloc(sizeof(int));//to avoid reading the same s with main process
                    *s_client = s;
                    pthread_create(&pid,NULL,receive,(void *)s_client);
                }
                break;
            case DISCONN:
                {
                    strcpy(msg_send,com_s[i]);//make msglen 6
                    send(s,msg_send,strlen(msg_send),0);
                    pthread_kill(pid,SIGQUIT);//when disconnted, receive thread need to be killed
                    close(s);//close server
                    s = -1;//set s to -1
                }
                break;                
            case TIME:
            case NAME:
            case LIST:
                {
                    strcpy(msg_send,com_s[i]);
                    send(s,msg_send,strlen(msg_send),0);
                }
                break;
            case SEND:
                {
                    strcpy(msg_send,com_s[i]);
                    scanf("%d",&send_id);//get id
//                    printf("id is %d\n",send_id);
                    char *tmp = msg_send+5;
                    while(send_id)
                    {
                        *tmp = send_id%10+'0';
                        send_id/=10;
                        tmp++;
                    }
  //                  printf("Message Sent is %s",msg_send);
                    send(s,msg_send,strlen(msg_send),0);//send com and id to server
                    //printf("please input your message ")
                    scanf("%s",msg_send);//get message
                    memset(msg_send+strlen(msg_send),' ',90-strlen(msg_send));//fill with space
                    msg_send[90] = '\0';
//                    printf("message sent is %s,len is %d",msg_send,(int)strlen(msg_send));
                    send(s,msg_send,90,0);
                }
                break;
            case QUIT:
                {
                    if(s!=-1)
                       {
                           printf("You are now connected, can not quit!\n");
                           continue;
                       }
                    else
                    {
                        printf("Welcome back!\nbye bye!\n");
                        return 0;
                    }
                }
                break;
        }
    }
    return 0;
}

void handle_quit(int signo)  //on close thread
{
    printf("DISCONN sucessfully!\n");
    pthread_exit(NULL);
}

void bail(char * on_what)//handle error message
{
    perror(on_what);
}

void decode_message(char * msg_recv,char *msg_type,char * from,char*message,char*seq)
{
        char *msg = msg_recv;
        strncpy(msg_type,msg,4);//get type
        msg_type[4] = '\0';
        msg = msg_recv+4;
        strncpy(from,msg,22);
        from[22] = '\0';
        msg = msg_recv+26;
        strncpy(message,msg,94);
        message[94] = '\0';
       // printf("%s\n",message);
        strncpy(seq,msg,2);
   //     printf("here seq is %s",seq);
        seq[2]='\0';
//      printf("here seq is %s\n",seq);
}

void decode_from(char * message, char *ip, char *port,char * id,int i)
{
    char * p;
    if(i==0)p= message+2;
    else
    {
        p = message;
    }
    strncpy(ip,p,15);
    p = p+15;
    strncpy(port,p,5);
    p = p+5;
    strncpy(id,p,2);
}

void *receive(void*id)  //receive message thread
{
    int z = -1,i;
    int client= *((int*)id);
    char msg_recv[MAX_MSG+1];//buffer
    char from[23];//IP AND PORT OF STRESS
    char msg_type[5];
    char IP[16];
    char PORT[6];
    char ID[3];
    char message[95];//FORMAT MESSAGE
    char *msg;//temp pointer
    const char com_c[][5] = {{" "},{"DISC"},{"TIME"},{"NAME"},{"LIST"},{"MESG"}};
    char seq[3];//message seq . if -1 then message end

    signal(SIGQUIT,handle_quit);
//    printf("I am a client\n");
//    printf("%d",father);
//   printf("%d\n",global);
    while(1)
    {
        if((z = recv(client,msg_recv,MAX_MSG,0))<=0)continue;
        msg_recv[z] = '\0';
//        printf("message receive is %s",msg_recv);
        //decode message
        //test type
        decode_message(msg_recv,msg_type,from,message,seq);
     //   printf("msg_type %s\n",msg_type);
        for (i = 0;i<6;i++)
        {
            if(!strcmp(msg_type,com_c[i]))break;
        }
   //     printf("decode type is %d\n",i);
        //print message
        switch(i)
        {
        case TIME:
        {
        printf("Time from server (%s) is %s\n",from,message);
        }
        break;
        case NAME:
        {
            printf("Name of Server (%s) is %s\n",from,message);
        }
        break;
        case SEND:
        {
            decode_from(from,IP,PORT,ID,1);
            printf("You've received a message from (IP:%s PORT:%s ID:%s):\n",IP,PORT,ID);
            //printf("%d\n",atoi(ID));
            printf("Message is %s\n",message);
        }
        break;
        case LIST:
        {
            int n = 1;
            printf("No  Address             port    id\n");
//            printf("seq is %s\n",seq);
            while(strcmp(seq,"-1")&&!strcmp(msg_type,"LIST"))
            {
                decode_from(message,IP,PORT,ID,0);
                printf("%d  %s      %s  %s\n",n++,IP,PORT,ID);
                //if(atoi(ID)==)//
                while((z = recv(client,msg_recv,MAX_MSG,0))<=0);
                msg_recv[z] = '\0';
               // printf("list receive in LIST circly is %s\n",msg_recv);
                decode_message(msg_recv,msg_type,from,message,seq);
            }
        }
        break;
        }
    }
}





