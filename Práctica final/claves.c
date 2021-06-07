#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lines.h"
#include "claves.h"
#include <limits.h>
#include <errno.h>

#define MAXSIZE 255
char buffer[MAXSIZE], *endptr;
int sd, err, res;

int conexion_socket() {

    int puerto;
    char *IP_TUPLAS;

    IP_TUPLAS = getenv("IP_TUPLAS");
    if (IP_TUPLAS == NULL) {
        printf("IP_TUPLAS no definida\n");
        return 0;
    }

    char *PORT_TUPLAS;
    PORT_TUPLAS = getenv("PORT_TUPLAS");
    if (PORT_TUPLAS == NULL) {
        printf("PORT_TUPLAS no definida\n");
        return 0;
    }

    puerto = atoi(PORT_TUPLAS);
    struct sockaddr_in server_addr;
    struct hostent *hp;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == 1) {
        printf("Error en socket\n");
        return -1;
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    hp = gethostbyname(IP_TUPLAS);
    if (hp == NULL) {
        printf("Error en gethostbyname\n");
        return -1;
    }

    memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(puerto);

    err = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("Error en connect\n");
        return -1;
    }
    return sd;
}

int init() {
    int sd = conexion_socket();

    sprintf(buffer, "%d", 0);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    close(sd);
    return res;
}

int set_value(char *key, char *value1, int value2, float value3) {
    int sd = conexion_socket();


    sprintf(buffer, "%d", 1);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, key);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, value1);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    sprintf(buffer, "%d", value2);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    sprintf(buffer, "%f", value3);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    close(sd);
    return res;
}

int get_value(char *key, char *value1, int *value2, float *value3) {
    int sd = conexion_socket();


    sprintf(buffer, "%d", 2);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, key);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    if (res == -1){
        close(sd);
        return res;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
    }
    strcpy(value1, buffer);

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    *value2 = strtol(buffer, &endptr, 10);

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    *value3 = atof(buffer);

    close(sd);
    return res;

}

int modify_value(char *key, char *value1, int value2, float value3) {
    int sd = conexion_socket();


    sprintf(buffer, "%d", 3);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, key);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, value1);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    sprintf(buffer, "%d", value2);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    sprintf(buffer, "%f", value3);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    close(sd);
    return res;
}

int delete_key(char *key) {
    int sd = conexion_socket();

    sprintf(buffer, "%d", 4);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, key);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    close(sd);
    return res;
}

int exist(char *key) {
    int sd = conexion_socket();

    sprintf(buffer, "%d", 5);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    strcpy(buffer, key);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    close(sd);
    return res;
}

int num_items() {
    int sd = conexion_socket();

    sprintf(buffer, "%d", 6);
    err = sendMessage(sd, buffer, strlen(buffer) + 1);
    if (err == -1) {
        printf("Error en envio\n");
        return -1;
    }

    err = readLine(sd, buffer, MAXSIZE);
    if (err == -1) {
        printf("Error en recepcion\n");
        return -1;
    }
    errno = 0;
    res = strtol(buffer, &endptr, 10);

    /* Comprobamos que lo que nos devuelve es un numero */
    if ((errno == ERANGE && (res == LONG_MAX || res == LONG_MIN)) || (errno != 0 && res == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (endptr == buffer) {
        fprintf(stderr, "No se encontraron digitos\n");
        exit(EXIT_FAILURE);
    }

    close(sd);
    return res;
}