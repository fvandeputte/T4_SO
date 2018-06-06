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
    serv_addr.sin_port = htons(atoi(argv[4]));
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0) 
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
    /// Mando el nickname
    unsigned char str1[20];
    printf("Enter nickname for playing: ");
    scanf("%s", str1);
    unsigned char message_with_nickname[50];
    message_with_nickname[0] = 4;
    message_with_nickname[1] = 20;
    for (int i=0; i < 20; i ++){
        message_with_nickname[2 + i] = str1[i];
    }
    send(sock, message_with_nickname , 50 * sizeof(unsigned char), 0);
    // Termino de mandar el nickname



    // PASO 5, me envien nickname contrincante
    while (message[0] != 5){
        valread = read(sock, message, 50);
    }
    char nickname_rival[message[1]];
    for (int n_caracter; n_caracter <  message[1]; n_caracter++){
        nickname_rival[n_caracter] = message[2 + n_caracter];
    }
    printf("¡Tienes rival! Su nombre es %s \n", nickname_rival);
    sleep(2);
    printf("\n\n\n\n");
    //Termino Paso 5






    //PASO 6 me dan me inicial pot
    while (message[0] != 6){
        valread = read(sock, message, 50);
    }
    int pot = 0;
    printf("En duro: %u \n", message[1]);
    int size = message[1];
    int j;
    printf("Tamaño: %i \n", size);
    for (int i= size - 1; i >= 0; i--){
        j = size - 1 - i;
        pot += power(256,i) * message[2 + j];
    }
    printf("¡Tu monto inicial es de: %i \n", pot);
    printf("\n\n\n\n");
    //Fin Paso 6

    
    unsigned char cartas[5][2];
    while (message[0] != 10){
        if (message[0] != last){
            valread = read(sock, message, 50);
            printf("Me llegaron las cartas\n");
            for (int i=0; i<5; i++) {
                unsigned char carta = message[2 + 2*i];
                unsigned char pinta = message[2 + 2*i + 1];
                printf("(%d) [%u, %u]\n", i+1, carta, pinta);
                cartas[i][0] = carta;
                cartas[i][1] = pinta;
            }
            sleep(1);
            break;
        }
    }

    // Leer 11 

    int primero = 0;
    while (message[0] != 11){
        if (message[0] != last){
            valread = read(sock, message, 50);
            if (message[2] == 1) {
                primero = 1;
            } else {
                primero = 0;
            }
            break;
        }
    }



    int n;
    int last_n = -1;
    int changes[5];
    int cont = 0;
    printf("¿Qué carta quieres cambiar? (un número entre 1 y 5, o f si ninguna más) ");
    while(scanf("%d",&n) == 1) {
        if (n != last_n && n >= 1 && n <= 5) {
            changes[cont++] = n;
            last_n = n;
        }
        if (n < 1 || n > 5) {
            printf("Número fuera del rango. ¿Qué carta quieres cambiar? (un número entre 1 y 5, o f si ninguna más) ");
        } else {
            printf("¿Qué carta quieres cambiar? (un número entre 1 y 5, o f si ninguna más) ");
        }
    }
    for (int i=0; i<cont; i++) {
        printf("%d ", changes[i]);
    }
    printf("\n");
    printf("Cambios son %d\n", cont);
    unsigned char* msg13 = calloc(2 + 2 * cont, sizeof(unsigned char));
    msg13[0] = 13;
    msg13[1] = 2 * cont;
    for (int i=0; i<cont; i++) {
        msg13[2 + 2 * i] = cartas[changes[i] - 1][0];
        msg13[2 + 2 * i + 1] = cartas[changes[i] - 1][1];
    }

    send(sock, msg13 , (2 + 2 * cont) * sizeof(unsigned char), 0);

    sleep(1);
    while (message[0] != 10){
        if (message[0] != last){
            valread = read(sock, message, 50);
            printf("Me llegaron las cartas con los cambios\n");
            for (int i=0; i<5; i++) {
                unsigned char carta = message[2 + 2*i];
                unsigned char pinta = message[2 + 2*i + 1];
                printf("(%d) [%u, %u]\n", i+1, carta, pinta);
                cartas[i][0] = carta;
                cartas[i][1] = pinta;
            }
            sleep(1);
            break;
        }
    }


    return 0;
}

