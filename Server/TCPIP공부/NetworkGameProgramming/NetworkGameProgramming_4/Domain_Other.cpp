#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>

using namespace std;

int main(int argc, char** argv)
{
    HOSTENT* hptr;
    IN_ADDR addr;
    int i = 0;

    char test[30];
    cin >> test;

    memset(&addr, 0, sizeof(addr));
    addr = inet_addr((char*)&test);

    hptr = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);

    if (!hptr)
    {
        printf("gethostbyaddr() error!\n");
        exit(1);
    }

    printf("Official Host Name : %s\n", hptr->h_name);

    for (i = 0; hptr->h_aliases[i] != NULL; i++)
        printf("Alias Host Name : %s\n", hptr->h_aliases[i]);

    for (i = 0; hptr->h_addr_list[i] != NULL; i++)
        printf("IP Address : %s\n", inet_ntoa(*(in_addr*)hptr->h_addr_list[i]));
}