#include <stdio.h>
#include <string.h>
#include <time.h>

char btbuf[121];
char *tmp;

int main()
{
    time_t t;
    time(&t);
    int z;
    strcpy(btbuf,"TIME");
    tmp = btbuf+4;
    memset(tmp,' ',22);
    tmp = tmp+22;
    strftime(tmp,94,"%A %b %d %H:%M:%S %Y\n",localtime(&t));
    printf("%s\n",btbuf);
i}

