#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int sock;
int busy = 0;

unsigned char get_num(char c) {
    if (c == '1') {
        return 1;
    }
    if (c == '2') {
        return 2;
    }
    if (c == '3') {
        return 3;
    }
    if (c == '4') {
        return 4;
    }
    if (c == '5') {
        return 5;
    }
    return 6;
}



void start_connnection(int sock){
    unsigned char message[3];
    message[0] = 1;
    message[1] = 0;
    message[2] = 0;
    printf("Enviando solicitud al servidor\n");
    send(sock, message , 3 * sizeof(unsigned char), 0);
    printf("Esperando respuesta ... \n");
}
int power(int base, unsigned int exp) {
    int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
 }
int imprimir_pos(unsigned char id){
    if(id == 1){
        printf("(1) FOLD\n");
    }
    else if (id == 2){
        printf("(2) $0\n");
    }
    else if (id == 3){
        printf("(3) $100\n");
    }
    else if (id == 4){
        printf("(4) $200\n" );
    }
    else{
        printf("(5) $500\n");

    }
}

unsigned char cartas[5][2];

void * handle_message(void * msg){
    unsigned char * message = ((unsigned char *) msg);
    int size;
    int j;
    int contador_cartas = 0;    
    if (message[0] == 2){
        printf("¡Tu conexión fue exitosa! Conectado al Servidor\n");
    }
    else if(message[0] == 3){
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
    }
    else if(message[0] == 5){
        char nickname_rival[message[1]];
        for (int n_caracter; n_caracter <  message[1]; n_caracter++){
            nickname_rival[n_caracter] = message[2 + n_caracter];
        }
        printf("¡Tienes rival! Su nombre es %s \n", nickname_rival);
    }
    else if (message[0] == 6){
        int pot = 0;
        size = message[1];
        for (int i= size - 1; i >= 0; i--){
            j = size - 1 - i;
            pot += power(256,i) * message[2 + j];
        }
        printf("[6] Tu monto inicial es de: %i \n", pot);
        printf("\n\n");
    }
    else if (message[0] == 7){
        printf("[7] Juego comienza correctamente\n");
        printf("\n\n");
    }
    else if(message[0] == 8){
        int actual_money = 0;
        size = message[1];
        for (int i= size - 1; i >= 0; i--){
            j = size - 1 - i;
            actual_money += power(256,i) * message[2 + j];
        }
        printf("[8] Tu plata actual es: %i \n", actual_money);
        printf("\n\n");
    }
    else if(message[0] == 9){
        int bet_obligatoria = 0;
        size = message[1];
        for (int i= size - 1; i >= 0; i--){
            j = size - 1 - i;
            bet_obligatoria += power(256,i) * message[2 + j];
        }
        printf("[9] Bet_inicial obligatoria es: %i \n", bet_obligatoria);
        printf("\n\n");
    }
    else if (message[0] == 10){
        printf("Cartas en la mano: \n");
        for (int i=0; i<5; i++) {
            unsigned char carta = message[2 + 2*i];
            unsigned char pinta = message[2 + 2*i + 1];
            printf("(%d) [%u, %u]\n", i+1, carta, pinta);
            cartas[i][0] = carta;
            cartas[i][1] = pinta;
        }

    }
    else if (message[0] == 12){
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
        unsigned char* msg13 = calloc(2 + 2 * cont, sizeof(unsigned char));
        msg13[0] = 13;
        msg13[1] = 2 * cont;
        for (int i=0; i<cont; i++) {
            msg13[2 + 2 * i] = cartas[changes[i] - 1][0];
            msg13[2 + 2 * i + 1] = cartas[changes[i] - 1][1];
            printf("i es %d, changes[i] es %d\n", i, changes[i]);
            printf("Poniendo en el mensaje la carta %u, %u\n", cartas[changes[i] - 1][0], cartas[changes[i] - 1][1]);
        }
        send(sock, msg13 , (2 + 2 * cont) * sizeof(unsigned char), 0);
    }
    else if (message[0] == 11){
        int primero = 0;
        if (message[2] == 1){
            printf("[11] Eres el jugador 1! Debes apostar primero \n");
        }
        else{
            printf("[11] Eres el jugador 2! Tienes que esperar la apuesta del jugador 1\n");
        }

    }
    else if (message[0] == 14){
        printf("message 14 es ");
        for (int i=0; i<message[1]; i++) {
            printf("%u ", message[i]);
        }
        printf("\n");
        sleep(1);
        int done14 = 0;
        unsigned char msg_bet[50];
        while (!done14) {
            printf("Tienes las siguientes apuestas disponibles: \n");
            for (int i= 0; i < message[1]; i ++){
                // printf("message[2+i] es %u\n", message[2 + i]);
                imprimir_pos(message[2+i]);
            }
            
            printf("¿Cuanto desea apostar? (inserte el numero de la apuesta) ");
            
            
            char c;
            c = getchar();
            c = getchar();
            c = getchar();
            printf("char es %c\n", c);
            msg_bet[0] = 15;
            msg_bet[1] = 1;
            msg_bet[2] = get_num(c);
            if (msg_bet[2] != 6) {
                done14 = 1;
            }
        }
        
        printf("msg_bet[2] es %u\n", msg_bet[2]);
        send(sock, msg_bet, 3, 0);
    }
    else if (message[0] == 17) {
        printf("OK BET\n");
    }
    else if (message[0] == 16) {
        printf("ERROR BET\n");
    }
    else if (message[0] == 18){
        printf("Ronda Termino\n\n\n");
    }
    else if(message[0] == 19){
        printf("Estas fueron las cartas de tu rival \n");
        unsigned char carta_rival;
        unsigned char pinta_rival;
        for (int i=0; i<5; i++) {
            carta_rival = message[2 + 2*i];
            pinta_rival = message[2 + 2*i + 1];
            printf("(%d) [%u, %u]\n", i+1, carta_rival, pinta_rival);

        }
    }
    else if (message[0] == 20){
        if (message[2] == 1){
            printf("Ganaste la ronda\n");
        }
        else {
            printf("Perdiste esta roonda\n");
        }
    }
}


