// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "funciones.c"
#define PORT 8080
  
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char * hello= "Hello from client";
    unsigned char message[50];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    //send(sock , hello , strlen(hello) , 0 );
    //start_connnection(sock);
    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    // printf("%s\n",buffer );
    int last = 0;
    // parto la Conexion
    start_connnection(sock);
    // Espero respuesta del servidor
    while (message[0] != 2){
        if (message[0] != last){
            valread = read(sock , message, 50);
        }
    }
    printf("¡Tu conexión fue exitosa! Conectado al Servidor\n");
    // espero que me pidan el nickname
    while (message[0] != 3){
        if (message[0] != last){
            valread = read(sock , message, 50);
        }
    }
    char str1[20];
    printf("Enter nickname for playing: ");
    scanf("%s", str1);

    return 0;
}

