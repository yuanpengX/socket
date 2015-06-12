#include "head.h"
#define default_addr "127.0.0.1"  
#define default_port "9090"
#define MAX_MSG 128
#define MAX_COM 7
int s=-1;
int z;
char  srvr_addr[80];//server address
char srvr_port[80];//server port
int len_inet;
char btbuf[128];
int id;
struct sockaddr_in addr_srvr;//server add

void bail(char* on_what)
{
    perror(on_what);
}

int decode_input(char * input)
{
    int flag;
    char command[MAX_COM][10] = {{"conn"},{"disconn"},{"time"},{"name"},{"list"},{"send"},{"quit"}};
    int i ;
    for(i = 0;i<MAX_COM;i++)
    {
        if(!strcmp(input,command[i]))
        {
            break;
        }
    }
    switch(i)
    {
        case 0:
            {
                scanf("%s %s",srvr_addr,srvr_port);
                flag = Connect();
            }
            break;
        case 1:
            {
               flag =  Disconn();
            }
            break;
        case 2:
            {
                flag = GetTime();
            }
            break;
        case 3:
            {
                flag = Name();
            }
            break;
        case 4:
            {
                flag = List();
            }
            break;
        case 5:
            {           
               scanf("%d",&id);
               flag =  Send();
            }
            break;
        case 6:flag = -1;break;
        default: flag = 0;
    }
    return flag;
}

int Disconn()
{
    strcpy(btbuf,"quit");
    z = send(s,btbuf,strlen(btbuf),0);
    close(s);
    s = -1;
    //printf("%d",z);
    return 1;
}


int GetTime()
{
    if(s==-1)
    {
        printf("you are not connected!\n");
    }
    strcpy(btbuf,"time   ");
    z = send(s,btbuf,strlen(btbuf),0);
    if(z>0)
    {
        z = recv(s,btbuf,MAX_MSG,0);
        btbuf[z] = '\0';
        printf("current time is %s\n",btbuf);
        return 1;
    }
    else
    {
        return 0;
    }
}

int Send()
{
    char id_c[3];
    id_c[2] ='\0'; 
    char *p = id_c+1;
    memset(id_c,'0',sizeof id_c);
    if(s==-1)
    {
        printf("You are not connected!\n");
    }
    else
    {
        while(id)
        {
            *p = id%10+'0';
            id/=10;
            p--;
        }
        strcpy(btbuf,"send   ");
        send(s,btbuf,strlen(btbuf),0);
        //send id
        send(s,id_c,strlen(id_c),0);  
        printf("please send you message:");
        scanf("%s",btbuf);
        send(s,btbuf,strlen(btbuf),0);
    }
}

int Name()
{
    if(s==-1){return 3;}
    else
    {
    strcpy(btbuf,"name   ");
    send(s,btbuf,strlen(btbuf),0);
    z = recv(s,btbuf,MAX_MSG,0);
    btbuf[z] = '\0';
    printf("host name is %s\n",btbuf);
    }
}

int List()
{ 
    if(s==-1)
    {
        return 3;
    }
    else
    {
        strcpy(btbuf,"list   ");
        send(s,btbuf,strlen(btbuf),0);
    }
    printf("addr               port         id\n");
    while(1)
    {
    z = recv(s,btbuf,24,0);//msg length is very important here
    if(z==-1) continue;
    btbuf[z] = '\0';
    if(!strcmp(btbuf,"end"))break;
    else
    {
        printf("%s\n",btbuf);
    }
    }
}

int Connect()
{
    s = socket(PF_INET,SOCK_STREAM,0);
    //set server addr
    memset(&addr_srvr,0,sizeof len_inet);
    addr_srvr.sin_family = PF_INET;
    addr_srvr.sin_port = htons(atoi(srvr_port));
    printf("addr is %s\n",srvr_addr);
    printf("port is %s\n",srvr_port);
    if(!inet_aton(srvr_addr,&addr_srvr.sin_addr))
    {
        bail("wrong address!");
        return 0;
    }
    len_inet = sizeof addr_srvr;
    z = connect(s,(struct sockaddr*)&addr_srvr,len_inet);
    if(z==-1)
    {
        bail("connect()");
        return 0;
    }
    return 1;
}

int main()
{
    char input[80];
    int ch;
    int z,g;
    printf("welcome to use our program! please enjoy yourself!\n");
    //get socket word
    while(1)
    {
        scanf("%s",input);
        system("clear");
        ch = decode_input(input);
        if(ch==-1)
        {
            break;
        }
        else if(ch == 3)
        {
            printf("You are not connected!\n");
        }
        }
    }

    //exit program
    printf("Wish you will soon come back!\nbye bye!\n");
    return 0;
}
