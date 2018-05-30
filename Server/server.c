// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "funciones_server.c"
#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    //                                               &opt, sizeof(opt)))
    // {
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( argv[4] );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int jugadores = 0;
    int sockets[2];
    unsigned char last_uno = 0; 
    unsigned char last_dos = 0;
    // unsigned char rpta[16];
    // int last_socket = -1;
    // while (1){

    //     // Continuamente estamos estamos escuchando sockets
    //     if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
    //                        (socklen_t*)&addrlen))<0)
    //     {
    //         perror("accept");
    //         exit(EXIT_FAILURE);
    //     }

    //     valread = read(new_socket,rpta, 16);
    //     if ((rpta[0] != 0 && rpta[0] != last) || new_socket != last_socket){
    //         last = rpta[0];
    //         printf("%u\n", rpta[0]);
    //         last_socket = new_socket;
    //     }

    // }
    while (1){ 
        // Acepto a los clientes
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                           (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
        }
        if (jugadores < 2){
            sockets[jugadores] = new_socket;
            jugadores ++;
        }
        // valread = read(new_socket, buffer, 1024);
        // printf("%s\n",buffer );
        // send(new_socket , hello , strlen(hello) , 0);
        // printf("Hello message sent\n");
        if (jugadores == 1){
            unsigned char mensaje_cliente[50];
            valread = read(sockets[0], mensaje_cliente, 50*sizeof(unsigned char));
            if (mensaje_cliente[0] != last_uno){
                handle_message(mensaje_cliente);
            }


        }
        else if(jugadores == 2){
            unsigned char mensaje_cliente_uno[50];
            unsigned char mensaje_cliente_dos[50];
            int m1 = read(sockets[0], mensaje_cliente_uno, 50*sizeof(unsigned char));
            int m2 = read(sockets[1], mensaje_cliente_dos, 50*sizeof(unsigned char));

        }

    }


    return 0;
}