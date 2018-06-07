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


int get_idx(unsigned char carta, unsigned char pinta, unsigned char mazo[52][2]) {
    int u = 0;
    while (mazo[u][0] != carta || mazo[u][1] != pinta) {
        u++;
    }
    return u;
}


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
    players[0].pot = 1000;
    players[1].pot = 1000;
    while (n_jugadores < 2){ 
        // Acepto a los clientes
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
    sleep(1);
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
    sleep(1);
    send(sockets[0], msg , 4 * sizeof(unsigned char), 0);
    send(sockets[1], msg , 4 * sizeof(unsigned char), 0);
    printf("Acabo de enviar inicio de juego correcto\n");

    // Paquete 7
    unsigned char msg7[2];
    msg7[0] = 7;
    msg7[1] = 0;
    sleep(1);
    send(sockets[0], msg7 , 2 * sizeof(unsigned char), 0);
    send(sockets[1], msg7 , 2 * sizeof(unsigned char), 0);
    printf("Acabo de enviar pot inicial de juego \n");


    int turno = rand() % 2;
    // Este while true porque hay muchas rondas: falta condicion termino
    while (1) {

        if (players[0].pot < 10 || players[1].pot < 10) {
            break;
            // se termina el juego, mandar ultimos paquetes
        }

        // Paquete 8
        sleep(1);
        start_round(sockets, players);
        printf("Acabo de enviar plata disponible \n");
        

        // Paquete 9 ---> Apuesta inicial (10)
        unsigned char msg9[3];
        msg9[0] = 9;
        msg9[1] = 1;
        msg9[2] = 10;
        sleep(1);
        send(sockets[0], msg9 , 3 * sizeof(unsigned char), 0);
        send(sockets[1], msg9 , 3 * sizeof(unsigned char), 0);
        sleep(1);
        printf("Acabo de enviar apuestas iniciales \n");

        // Paquete 10 --> Cartas

        int count = 0;
        int cartas_idxs[10];
        for (int i=0; i<10; i++) {
            cartas_idxs[i] = -1;
        }
        while (count < 10) {
            int carta_idx =  rand() % 52;
            if (not_picked(carta_idx, cartas_idxs, count)) {
                cartas_idxs[count++] = carta_idx;
            }
        }
        unsigned char msg10[12];
        msg10[0] = 10;
        msg10[1] = 10;
        for (int i=0; i<5; i++) {
            for (int j=0; j<2; j++) {
                msg10[2 + 2 * i + j] = mazo[cartas_idxs[i]][j];
            }
        }
        sleep(1);
        send(sockets[0], msg10 , 12 * sizeof(unsigned char), 0);
        for (int i=0; i<5; i++) {
            for (int j=0; j<2; j++) {
                msg10[2 + 2 * i + j] = mazo[cartas_idxs[5 + i]][j];
            }
        }
        send(sockets[1], msg10 , 12 * sizeof(unsigned char), 0);
        // Fin Paquete 10: envio de cartas


        
        // Paquete 12 --> cartas para cambiar //
        unsigned char msg12[2];
        msg12[0] = 12;
        msg12[1] = 0;
        sleep(1);
        send(sockets[0], msg12 , 2 * sizeof(unsigned char), 0);
        send(sockets[1], msg12 , 2 * sizeof(unsigned char), 0);


        // Paquete 13: este se lee, no se manda

        unsigned char message[12];
        valread = read(sockets[0], message, 12);
        printf("Mensaje en 0 del primero: %u \n", message[0]);
        if (message[0] == 13) {
            unsigned char nro_cartas = message[1] / 2;
            printf("Tengo que cambiar %d cartas\n", nro_cartas);
            printf("message es ");
            for (int i=0; i<message[1]; i++) {
                printf("%u ", message[i]);
            }
            printf("\n");
            for (int cambio = 0; cambio<nro_cartas; cambio++) {
                unsigned char carta = message[2 + 2 * cambio];
                unsigned char pinta = message[2 + 2 * cambio + 1];
                printf("carta es %d, pinta es %d\n", carta, pinta);
                int idx_carta = get_idx(carta, pinta, mazo);
                int p = 0;
                while (cartas_idxs[p] != idx_carta) {
                    p++;
                }
                printf("p es %d\n", p);                
                int carta_idx =  rand() % 52;
                while (!not_picked(carta_idx, cartas_idxs, 10)) {
                    carta_idx =  rand() % 52;
                }
                cartas_idxs[p] = carta_idx;
                printf("Tengo que cambiar %u, %u; p es %d; carta ahora es %u, %u\n", carta, pinta, p, mazo[cartas_idxs[p]][0], mazo[cartas_idxs[p]][1]);
            }

            // Enviar nuevo paquete 10 con las cartas actualizadas
            unsigned char msg10a[12];
            msg10a[0] = 10;
            msg10a[1] = 12;
            for (int i=0; i<5; i++) {
                for (int j=0; j<2; j++) {
                    msg10a[2 + 2 * i + j] = mazo[cartas_idxs[i]][j];
                }
            }
            int valid_sent = send(sockets[0], msg10a, 12 * sizeof(unsigned char), 0);
            printf("Valid sent: \n");
      
        }
  
        valread = read(sockets[1], message, 20);
        printf("Mensaje en 0 del segundo, es: %u\n", message[0]);

        if (message[0] == 13) {
            unsigned char nro_cartas = message[1] / 2;
            printf("Tengo que cambiar %d cartas\n", nro_cartas);
            
            printf("message es ");
            for (int i=0; i<message[1]; i++) {
                printf("%u \n", message[i]);
            }
            printf("\n");

            for (int cambio = 0; cambio<nro_cartas; cambio++) {
                unsigned char carta = message[2 + 2 * cambio];
                unsigned char pinta = message[2 + 2 * cambio + 1];
                int idx_carta = get_idx(carta, pinta, mazo);
                int p = 0;
                while (cartas_idxs[p] != idx_carta) {
                    p++;
                }
                printf("p es %d\n", p);
                int carta_idx =  rand() % 52;
                while (!not_picked(carta_idx, cartas_idxs, 10)) {
                    carta_idx =  rand() % 52;
                }
                cartas_idxs[p] = carta_idx;
                printf("Tengo que cambiar %u, %u; p es %d; carta ahora es %u, %u\n", carta, pinta, p, mazo[cartas_idxs[p]][0], mazo[cartas_idxs[p]][1]);
            }

            // Enviar nuevo paquete 10 con las cartas actualizadas
            unsigned char msg10b[12];
            msg10b[0] = 10;
            msg10b[1] = 12;
            for (int i=0; i<5; i++) {
                for (int j=0; j<2; j++) {
                    msg10b[2 + 2 * i + j] = mazo[cartas_idxs[5 + i]][j];
                }
            }
            sleep(2);
            send(sockets[1], msg10b, 12 * sizeof(unsigned char), 0);
            
        } else {
            printf("Llegó paquete %u\n", message[0]);
        }

        // Paquete 11 --> Turnos

        unsigned char msg11[3];
        msg11[0] = 11;
        msg11[1] = 1;
        if (turno == 0) {
            msg11[2] = 1;
        } else {
            msg11[2] = 2;
        }
        sleep(3);
        send(sockets[0], msg11 , 3 * sizeof(unsigned char), 0);
        if (turno == 0) {
            msg11[2] = 2;
        } else {
            msg11[2] = 1;
        }
        send(sockets[1], msg11 , 3 * sizeof(unsigned char), 0);
        printf("Acabo de enviar los turnos \n");



        // Paquetes 14, 15, 16 y 17
        
        
        sleep(1);
        int BETS[] = {-1, 0, 0, 100, 200, 200}; //-1 es nada, es para que parta en indice 1
        int done1 = 0;
        int winner = 99;
        unsigned char bet_id1;
        unsigned char bet_id2;
        while (!done1) {
            printf("Not done1\n");
            int sup;
            if (players[turno].pot >= 200) {
                sup = 7;
            } else if(players[turno].pot >= 200) {
                sup = 6;
            } else if(players[turno].pot >= 100) {
                sup = 5;
            } else {
                sup = 4;
            }
            unsigned char * msg14a = calloc(sup, sizeof(unsigned char));
            msg14a[0] = 14;
            msg14a[1] = 5;
            msg14a[2] = 1;
            msg14a[3] = 2;
            if (sup > 4) {
                msg14a[4] = 3;
            }
            if (sup > 5) {
                msg14a[5] = 4;
            }
            if (sup > 6) {
                msg14a[6] = 5;
            }
            send(sockets[turno], msg14a, sup * sizeof(unsigned char), 0);

            valread = read(sockets[turno], message, 20);
            if (message[0] == 15) {
                bet_id1 = message[2];
                printf("bet_id1 es %u\n", bet_id1);
            }
            if (bet_id1 == 1) {
                winner = 1 - turno;
                msg17[0] = 17;
                msg17[1] = 0;
                done1 = 1;
                send(sockets[turno], msg17, 2 * sizeof(unsigned char), 0);
            }
            if (winner == 99) {
                unsigned char msg17[2];
                unsigned char msg16[2];
                if (turno == 0) {
                    if (bet_id1 > 2 && BETS[bet_id1] <= players[0].pot) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done1 = 1;
                        send(sockets[0], msg17, 2 * sizeof(unsigned char), 0);
                    } else if (bet_id1 <= 2) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done1 = 1;
                        send(sockets[0], msg17, 2 * sizeof(unsigned char), 0);
                    } else {
                        msg16[0] = 16;
                        msg16[1] = 0;
                        send(sockets[0], msg16, 2 * sizeof(unsigned char), 0);
                    }
                } else {
                    if (bet_id1 > 2 && BETS[bet_id1] <= players[1].pot) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done1 = 1;
                        send(sockets[1], msg17, 2 * sizeof(unsigned char), 0);
                    } else if (bet_id1 <= 2) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done1 = 1;
                        send(sockets[1], msg17, 2 * sizeof(unsigned char), 0);
                    } else {
                        msg16[0] = 16;
                        msg16[1] = 0;
                        send(sockets[1], msg16, 2 * sizeof(unsigned char), 0);
                    }
                }
            }
        }
        sleep(1);
        int done2 = 0;
        while (!done2 && winner == 99) {
            printf("Not done2 and no winner yet\n");

            int count2 = 1;
            for (int i=2; i < 6; i++) {
                if (BETS[i] <= players[turno].pot && (i >= bet_id1 || bet_id1 == 1)) {
                    count2++;
                }
            }
            printf("ACA1\n");
            unsigned char msg14b[2 + count2];
            msg14b[0] = 14;
            msg14b[1] = count2;
            for (int i=0; i<count2; i++) {
                if (BETS[i] <= players[turno].pot && (i >= bet_id1 || bet_id1 == 1)) {
                    msg14b[2 + i] = i;
                }
            }
            send(sockets[1 - turno], msg14b, count2 * sizeof(unsigned char), 0);

            valread = read(sockets[1 - turno], message, 20);
            printf("ACA2\n");
            if (message[0] == 15) {
                bet_id2 = message[2];
            }
            if (bet_id2 == 1) {
                winner = turno;
            }
            if (winner == 99) {
                unsigned char msg17[2];
                unsigned char msg16[2];
                if (turno == 1) {
                    if (bet_id2 > 2 && BETS[bet_id2] <= players[0].pot) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done2 = 1;
                        send(sockets[0], msg17, 2 * sizeof(unsigned char), 0);
                    } else if (bet_id2 <= 2) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done2 = 1;
                        send(sockets[0], msg17, 2 * sizeof(unsigned char), 0);
                    } else {
                        msg16[0] = 16;
                        msg16[1] = 0;
                        send(sockets[0], msg16, 2 * sizeof(unsigned char), 0);
                    }
                } else {
                    if (bet_id2 > 2 && BETS[bet_id2] <= players[1].pot) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done2 = 1;
                        send(sockets[1], msg17, 2 * sizeof(unsigned char), 0);
                    } else if (bet_id2 <= 2) {
                        msg17[0] = 17;
                        msg17[1] = 0;
                        done2 = 1;
                        send(sockets[1], msg17, 2 * sizeof(unsigned char), 0);
                    } else {
                        msg16[0] = 16;
                        msg16[1] = 0;
                        send(sockets[1], msg16, 2 * sizeof(unsigned char), 0);
                    }
                }
            }
        }
        

        // Paquete 18
        unsigned char msg18[2];
        msg18[0] = 18;
        msg18[1] = 0;
        send(sockets[0], msg18, 2 * sizeof(unsigned char), 0);
        send(sockets[1], msg18, 2 * sizeof(unsigned char), 0);

        // Paquete 19

        unsigned char msg19[12];
        msg19[0] = 19;
        msg19[1] = 10;
        for (int i=0; i<5; i++) {
            msg19[2 + 2*i] = mazo[cartas_idxs[5 + i]][0];
            msg19[2 + 2*i + 1] = mazo[cartas_idxs[5 + i]][1];
        }
        send(sockets[0], msg19, 12 * sizeof(unsigned char), 0);

        for (int i=0; i<5; i++) {
            msg19[2 + 2*i] = mazo[cartas_idxs[i]][0];
            msg19[2 + 2*i + 1] = mazo[cartas_idxs[i]][1];
        }
        send(sockets[1], msg19, 12 * sizeof(unsigned char), 0);

        // Paquete 20
        if (winner == 99) {
            float* scores = get_scores(cartas_idxs, mazo);
            if (scores[0] >= scores[1]) {
                winner = 0;
            } else {
                winner = 1;
            }
            printf("Scores son %f, %f\n", scores[0], scores[1]);
        } else { // winner se definió antes, en caso fold, respecto a turno, no respecto a cada jugador
            if (turno == 1) {
                winner = 1 - winner;
            }
        }
        unsigned char msg20[3];
        msg20[0] = 20;
        msg20[1] = 1;
        if (winner == 0) {
            msg20[2] = 1;
        } else {
            msg20[2] = 0;
        }
        send(sockets[0], msg20, 3 * sizeof(unsigned char), 0);
        msg20[2] = 1 - msg20[2];
        send(sockets[1], msg20, 3 * sizeof(unsigned char), 0);


        // Paquete 21
        // tenemos bet_id1 y bet_id2 (por turnos: bet_id1 del q jugo primero), tenemos winner (por jugador: el jugador 1 es 0, el segundo es 1): actualizar players y mandarlos
        if (turno == 0) {
            players[winner].pot += bet_id2;
            players[1 - winner].pot -= bet_id1;
        } else {
            players[winner].pot += bet_id1;
            players[1 - winner].pot += bet_id2;
        }
        unsigned char msg21[4];
        msg21[0] = 21;
        msg21[1] = 2;
        msg21[2] = players[0].pot / 256;
        msg21[3] = players[0].pot % 256;
        send(sockets[0], msg21, 4 * sizeof(unsigned char), 0);

        sleep(1);
        msg21[2] = players[1].pot / 256;
        msg21[3] = players[1].pot % 256;
        send(sockets[1], msg21, 4 * sizeof(unsigned char), 0);

    }
    
    // Paquete 22
    unsigned char msg22[2];
    msg22[0] = 22;
    msg22[1] = 0;
    send(sockets[0], msg22, 2 * sizeof(unsigned char), 0);
    send(sockets[1], msg22, 2 * sizeof(unsigned char), 0);


    // Paquete 23


    return 0;
}


