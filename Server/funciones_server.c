// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "estructura.h"
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

 void * manejar_conexion_y_nickname(void *vargp){
    

    int socket = *((int *) vargp);
    printf("Socket: %i\n", socket);
    unsigned char message[50];
    int valread = read(socket, message, 50*sizeof(unsigned char));
    if (message[0] == 1){
        // MENSAJE DE ENVIO DE CONEXION EXITOSA --> PASO 2 
        unsigned char message[3];
        message[0] = 2;
        message[1] = 0;
        message[2] = 0;
        printf("Se conecto jugador\n");
        send(socket, message , 3 * sizeof(unsigned char), 0);
        printf("Enviando conexion ...\n");
        // FIN MENSAJE ENVIO EXITOSO
        // AQUI ENVIO SOLICITUD DE NICKNAME ---> PASO 3
        unsigned char message_nickname[3];
        message_nickname[0] = 3;
        message_nickname[1] = 0;
        message_nickname[2] = 0;
        send(socket, message_nickname , 3 * sizeof(unsigned char), 0);
        // AQUI ESPERO DE VUELTA EL NICKNAME --> PASO 4
        printf("Esperando que usuario ingrese nombre... \n");
        int valread = read(socket, message, 50 * sizeof(unsigned char));
        //pl -> nickname = malloc(sizeof(char)*20);
        printf("Ingreso algo \n");
        char * nickname = malloc(sizeof(char)* 20);
        for (int i=0; i < 20; i++){
            nickname[i] = message[2 + i];
        }
        //pl -> nickname = malloc(sizeof(char)*20);
        //pl -> nickname = nickname;
        printf("nickname escogido: %s\n", nickname);
        return nickname;
    }  
 }