// Client side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "math.h"
#include "funciones.c"
#include <arpa/inet.h>
#include <wchar.h>
#include <locale.h>
#define PORT 8080



int main(int argc, char const *argv[])
{
    setlocale(LC_CTYPE, "");

    sleep(2); // se demora un poco la conexion al server
    struct sockaddr_in address;
    sock = 0;
    int valread;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;

    if (strcmp(argv[3], "-p") == 0) {
        serv_addr.sin_port = htons(atoi(argv[4]));
    } else {
        serv_addr.sin_port = htons(atoi(argv[2]));
    }
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (strcmp(argv[1], "-i") == 0) {
        if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0) 
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }
    } else {
        if(inet_pton(AF_INET, argv[4], &serv_addr.sin_addr)<=0) 
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }
    }
    
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    start_connnection(sock);
    while (1){
        unsigned char *message;
        message = malloc(sizeof(unsigned char)*20);
        valread = read(sock , message, 20);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_message, message);

    }
    //send(sock , hello , strlen(hello) , 0 );
    //start_connnection(sock);
    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    // printf("%s\n",buffer );  
}

