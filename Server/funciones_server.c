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
        sleep(1);
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
        sleep(1);
        int valid_sent = send(socket, message_nickname , 3 * sizeof(unsigned char), 0);
        printf("Valid sent: %i\n", valid_sent);
        // AQUI ESPERO DE VUELTA EL NICKNAME --> PASO 4
        printf("Esperando que usuario ingrese nombre... \n");
        int valread = read(socket, message, 50 * sizeof(unsigned char));
        //pl -> nickname = malloc(sizeof(char)*20);
        char * nickname = malloc(sizeof(char)* 20);
        for (int i=0; i < 20; i++){
            nickname[i] = message[2 + i];
        }
        //pl -> nickname = malloc(sizeof(char)*20);
        //pl -> nickname = nickname;
        return nickname;
    }  
 }




void start_round(int sockets[2], Jugador players[2]) {
    unsigned char msg[4];
    msg[0] = 8;
    msg[1] = 2; //max plata que podis tener es 65535, razonable
    msg[2] = players[0].pot / 256;
    msg[3] = players[0].pot % 256;
    send(sockets[0], msg , 4 * sizeof(unsigned char), 0);

    msg[2] = players[1].pot / 256;
    msg[3] = players[1].pot % 256;
    send(sockets[1], msg , 4 * sizeof(unsigned char), 0);

}   

// unsigned char * get_card() {
//     static unsigned char card[2];
//     int carta = 1 + rand() % 13;
//     int pinta = 1 + rand() % 4;
//     card[0] = carta;
//     card[1] = pinta;
//     return card;
// }


int not_picked(int carta_idx, int cartas_idxs[20], int count) {
    for (int i=0; i<count; i++) {
        if (carta_idx == cartas_idxs[i]) {
            return 0;
        }
    }
    return 1;
}


int has_straight(unsigned char cartas[5][2]) {
    for (int i=0; i<5; i++) {
        int neighbour = 0;
        for (int j=0; j<5; j++) {
                if (cartas[i][0] == cartas[j][0] + 1 || cartas[i][0] == cartas[j][0] - 1) {
                    neighbour = 1;
                }
        }
        if (neighbour == 0) {
            return 0;
        }
    }
    return 1;
}

int has_flush(unsigned char cartas[5][2]) {
    for (int i=1; i<5; i++) {
        if (cartas[0][1] != cartas[i][1]) {
            return 0;
        }
    }
    return 1;
}


int has_straight_flush(unsigned char cartas[5][2]) {
    return has_straight(cartas) && has_flush(cartas);
}


int has_four(unsigned char cartas[5][2]) {
    unsigned char carta1 = 15;
    unsigned char carta2 = 18;
    int count1;
    int count2;
    for (int i=0; i<5; i++) {
        if (carta1 == 15) {
            carta1 = cartas[i][0];
            count1++;
        } else if (cartas[i][0] != carta1 && carta2 == 18) {
            carta2 = cartas[i][0];
            count2++;
        } else {
            if (cartas[i][0] == carta1) {
                count1++;
            } else if (cartas[i][0] == carta2) {
                count2++;
            }
        }
    }
    if (count1 == 4) {
        return carta1;
    } else if (count2 == 4) {
        return carta2;
    }
    return 0;
}

int has_three(unsigned char cartas[5][2]) {
    for (int i=0; i<5; i++) {
        unsigned char carta_aux = cartas[i][0];
        int cont_aux = 0;
        for (int j=i; j<5; j++) {
            if (carta_aux == cartas[j][0]) {
                cont_aux++;
            }
        }
        if (cont_aux == 3) {
            return carta_aux;
        }
    }
    return 0;
}

int has_two_pairs(unsigned char cartas[5][2]) {
    int cont_aux = 0;
    int max = 0;
    for (int i=0; i<5; i++) {
        unsigned char carta_aux = cartas[i][0];
        for (int j=i; j<5; j++) {
            if (i != j && carta_aux == cartas[j][0]) {
                cont_aux++;
                if (carta_aux > max) {
                    max = carta_aux;
                }
            }
        }
        if (cont_aux == 2) {
            return max;
        }
    }
    return 0;
}

int has_pair(unsigned char cartas[5][2]) {
    for (int i=0; i<4; i++) {
        unsigned char carta_aux = cartas[i][0];
        for (int j=i+1; j<5; j++) {
            if (carta_aux == cartas[j][0]) {
                return carta_aux;
            }
        }
    }
    return 0;
}

int highest_card(unsigned char cartas[5][2]) {
    int max = 0;
    for (int i=0; i<5; i++) {
        if (cartas[i][0] > max) {
            max = cartas[i][0];
        }
    }
    return max;
}

float has_full(unsigned char cartas[5][2]) {
    unsigned char carta1 = cartas[0][0];
    unsigned char carta2 = 99;
    int count1 = 1;
    int count2;

    for (int i=1; i<5; i++) {
        if (cartas[i][0] != carta1 && carta2 == 99) {
            carta2 = cartas[i][0];
            count2++;
        } else if (cartas[i][0] == carta1) {
            count1++;
        } else if (cartas[i][0] == carta2) {
            count2++;
        }
    }

    if ((count1 == 3 && count2 == 2)) {
        return (carta1 * 14 + carta2) / 14;
    } else if ((count1 == 2 && count2 == 3)) {
        return (carta2 * 14 + carta1) / 14;
    }
    return 0;
}

float * get_scores(int cartas_idxs[10], unsigned char mazo[52][2]) {
    unsigned char cartas1[5][2];
    for (int i=0; i<5; i++) {
        cartas1[i][0] = mazo[cartas_idxs[i]][0];
        cartas1[i][1] = mazo[cartas_idxs[i]][1];
    }

    unsigned char cartas2[5][2];
    for (int i=0; i<5; i++) {
        cartas2[i][0] = mazo[cartas_idxs[5 + i]][0];
        cartas2[i][1] = mazo[cartas_idxs[5 + i]][1];
    }

    static float scores[2] = {0, 0};

    if (has_straight_flush(cartas1)) {
        scores[0] = 200 + highest_card(cartas1);
    } else if (has_four(cartas1) > 0) {     
        scores[0] = 180 + has_four(cartas1);
    } else if (has_full(cartas1) > 0.0) {
        scores[0] = 160.0 + has_full(cartas1);
    } else if (has_flush(cartas1) > 0) {
        scores[0] = 120 + has_flush(cartas1);
    } else if (has_straight(cartas1)) {
        scores[0] = 100 + highest_card(cartas1);
    } else if (has_three(cartas1) > 0) {
        scores[0] = 100 + has_three(cartas1);
    } else if (has_two_pairs(cartas1) > 0) {
        scores[0] = 60 + has_two_pairs(cartas1);
    } else if (has_pair(cartas1) > 0) {
        scores[0] = 20 + has_pair(cartas1);
    } else {
        scores[0] = highest_card(cartas1);
    }
    

    if (has_straight_flush(cartas2)) {
        scores[1] = 200 + highest_card(cartas2);
    } else if (has_four(cartas2) > 0) {     
        scores[1] = 180 + has_four(cartas2);
    } else if (has_full(cartas2) > 0) {
        scores[1] = 160 + has_full(cartas2);
    } else if (has_flush(cartas2) > 0) {
        scores[1] = 120 + has_flush(cartas2);
    } else if (has_straight(cartas2)) {
        scores[1] = 100 + highest_card(cartas2);
    } else if (has_three(cartas2) > 0) {
        scores[1] = 100 + has_three(cartas2);
    } else if (has_two_pairs(cartas2) > 0) {
        scores[1] = 60 + has_two_pairs(cartas2);
    } else if (has_pair(cartas2) > 0) {
        scores[1] = 20 + has_pair(cartas2);
    } else {
        scores[1] = highest_card(cartas2);
    }
    return scores;

}



