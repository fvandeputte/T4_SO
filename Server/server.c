// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "funciones_server.c"
#include <pthread.h>
#define PORT 8080


unsigned char mazo[52][2] = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1}, {12, 1}, {13, 1}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2}, {12, 2}, {13, 2}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3}, {12, 3}, {13, 3}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4}, {12, 4}, {13, 4}};


// unsigned char[52][2] create_mazo() {
//     unsigned char aux_maxo[52][2];
//     for (int i=0; i<4; i++) {
//         for (int j=0; j<13; j++) {
//             aux_mazo[13*i + j][0] = j;
//             aux_mazo[13*i + j][1] = i;
//         }
//     }
//     return aux_mazo
// }





int main(int argc, char const *argv[])
{
    srand(time(NULL)); //set seed
    int server_fd, valread;
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
    address.sin_port = htons(atoi(argv[4]));
      
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
    int n_jugadores = 0;
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



    int m1, m2;
    pthread_t threads[2];
    Jugador players[2];
    while (n_jugadores < 2){ 
        // Acepto a los clientes
        printf("jugadores: %i \n", n_jugadores);
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                           (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
        }
        pthread_t thread_id;
        sockets[n_jugadores] = new_socket;
        int *i = malloc(sizeof(*i));
        *i = new_socket;
        pthread_create(&thread_id, NULL, manejar_conexion_y_nickname, i);
        threads[n_jugadores] = thread_id;
        n_jugadores ++;


    }
    /// AQUI ESPERAMOS QUE LOS 2 HAYAN ELEGIDO NICKNAME //
    char * nick_uno;
    char * nick_dos;
    pthread_join(threads[0], (void**)&nick_uno);
    pthread_join(threads[1], (void**)&nick_dos);

    /// Estan los 2 con nombre, tengo que cruzarlos y enviarlos
    
    // 1er paquete 5
    unsigned char message1[22];
    message1[0] = 5;
    message1[1] = 20;
    for (int i = 0; i < 20; i++)
    {
        message1[2+i] = nick_dos[i];
    }
    send(sockets[0], message1 , 22 * sizeof(unsigned char), 0);
    
    // 2do paquete 5
    unsigned char message2[22];
    message2[0] = 5;
    message2[1] = 20;
    for (int i = 0; i < 20; i++)
    {
        message2[2+i] = nick_uno[i];
    }
    send(sockets[1], message2 , 22 * sizeof(unsigned char), 0);

    // Paquete 6
    unsigned char msg[4];
    msg[0] = 6;
    msg[1] = 2;
    msg[2] = 1000 / 256;
    msg[3] = 1000 % 256;
    send(sockets[0], msg , 4 * sizeof(unsigned char), 0);
    send(sockets[1], msg , 4 * sizeof(unsigned char), 0);

    // Paquete 7
    unsigned char msg7[2];
    msg7[0] = 7;
    msg7[1] = 0;
    send(sockets[0], msg7 , 2 * sizeof(unsigned char), 0);
    send(sockets[1], msg7 , 2 * sizeof(unsigned char), 0);


    int turno = 1;
    // Este while true porque hay muchas rondas: falta condicion termino
    // while (1) {

        // if (players[0].inicial_pot < 10 || players[1].inicial_pot < 10) {
        //     break;
        //     // se termina el juego, mandar ultimos paquetes
        // }

        // Paquete 8
        start_round(sockets, players);

        // Paquete 9
        unsigned char msg9[3];
        msg9[0] = 9;
        msg9[1] = 1;
        msg9[2] = 10;
        send(sockets[0], msg9 , 3 * sizeof(unsigned char), 0);
        send(sockets[1], msg9 , 3 * sizeof(unsigned char), 0);
        sleep(2);

        // Paquete 10

        int count = 0;
        int cartas_idxs[20];
        for (int i=0; i<20; i++) {
            cartas_idxs[i] = -1;
        }


        while (count < 10) {
            int carta_idx =  rand() % 52;
            if (not_picked(carta_idx, cartas_idxs, count)) {
                cartas_idxs[count++] = carta_idx;
            }
        }


        sleep(2);
        unsigned char msg10[12];
        msg10[0] = 10;
        msg10[1] = 10;
        for (int i=0; i<5; i++) {
            for (int j=0; j<2; j++) {
                msg10[2 + 2 * i + j] = mazo[cartas_idxs[i]][j];
            }
        }

        send(sockets[0], msg10 , 12 * sizeof(unsigned char), 0);
        sleep(2);
        for (int i=0; i<5; i++) {
            for (int j=0; j<2; j++) {
                msg10[2 + 2 * i + j] = mazo[cartas_idxs[5 + i]][j];
            }
        }

        send(sockets[1], msg10 , 12 * sizeof(unsigned char), 0);


        // Paquete 11
        unsigned char msg11[3];
        msg11[0] = 11;
        msg11[1] = 1;
        msg11[2] = turno;
        send(sockets[0], msg11 , 3 * sizeof(unsigned char), 0);
        turno = 3 - turno;
        send(sockets[1], msg11 , 3 * sizeof(unsigned char), 0);

        // Paquete 12
        unsigned char msg12[2];
        msg12[0] = 12;
        msg12[1] = 0;
        send(sockets[0], msg12 , 2 * sizeof(unsigned char), 0);
        send(sockets[1], msg12 , 2 * sizeof(unsigned char), 0);


    // }
    



    return 0;
}