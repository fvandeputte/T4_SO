// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

void handle_message(int jugadores, int socket, unsigned char message[50]){
    printf("Handling message: %u \n", message[0]);
    if (message[0] == 1){
        unsigned char message[3];
        message[0] = 2;
        message[1] = 0;
        message[2] = 0;
        if (jugadores == 1){
            printf("Se conecto el primer jugador\n");
            send(socket, message , 3 * sizeof(unsigned char), 0);
            printf("Enviando conexion ...\n");
        }
        else if(jugadores == 2){
            printf("Se conecto el segundo jugador\n");
            send(socket, message , 3 * sizeof(unsigned char), 0);
            printf("Enviando conexion ...\n");
        }
    }  
}