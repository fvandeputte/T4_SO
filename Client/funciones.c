#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

void start_connnection(int sock){
    unsigned char message[3];
    message[0] = 1;
    message[1] = 0;
    message[2] = 0;
    send(sock, message , 3 * sizeof(unsigned char), 0);
}