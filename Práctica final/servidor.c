#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "lines.h"
#include "lista.h"
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>
#include "rpc.h"

userList users;
messageList messages;
#define MAX_THREADS 6
#define MAXSIZE 255
pthread_mutex_t m;
pthread_cond_t c;
int busy = true;
char buffer[MAXSIZE], *endptr;
char IP_RPC[16];
int res, err;
struct sockaddr_in server_addr, client_addr;
struct hostent *hp;

void servicio(void *);

int main(int argc, char *argv[])
{
    int sc, sd, val;
    pthread_attr_t attr;

    pthread_t thid;
    socklen_t size;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (argc != 5)
    {

        printf("Falta o sobra un argumento\n");

        return -1;
    }

    if (strcmp(argv[1], "-p") != 0)
    {

        printf("Flag incorrecto\n");

        return -1;
    }
    if (strcmp(argv[3], "-s") != 0)
    {

        printf("Flag incorrecto\n");

        return -1;
    }

    int puerto = atoi(argv[2]);

    strcpy(IP_RPC, argv[4]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SERVER: Error en el socket");
        return (0);
    }
    val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(puerto);
    printf("init server %s:%d\n", inet_ntoa(server_addr.sin_addr), server_addr.sin_port);

    err = bind(sd, (const struct sockaddr *)&server_addr,
               sizeof(server_addr));
    if (err == -1)
    {
        printf("Error en bind\n");
        return -1;
    }

    err = listen(sd, SOMAXCONN);
    if (err == -1)
    {
        printf("Error en listen\n");
        return -1;
    }

    size = sizeof(client_addr);

    while (1)

    { 
        sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);

        if (sc == -1)
        {
            printf("Error en accept\n");
            return -1;
        }
        
        //printf("conexion aceptada de IP: %s   Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_create(&thid, &attr, (void *)servicio, (void *)&sc);
        pthread_mutex_lock(&m);

        while (busy == true)
        {
            pthread_cond_wait(&c, &m);
        }

        busy = true;
        pthread_mutex_unlock(&m);
    }

    close(sd);
    return (0);
}

void servicio(void *sd)
{
    int sc;
    char res;
    char user[MAXSIZE];
    char user2[MAXSIZE];
    char message[MAXSIZE];
    char op[MAXSIZE];
    char port[MAXSIZE];
    char IP[MAXSIZE];
    CLIENT *clnt;
    enum clnt_stat retval;
    struct username user_registered;
    int response;

    pthread_mutex_lock(&m);
    sc = (*(int *)sd);
    int soc_desc;
    int soc_dest;
    busy = false;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);

    /* Leemos el codigo de operacion del buffer */
    err = readLine(sc, buffer, MAXSIZE);
    strcpy(op, buffer);
    if (err == -1)
    {
        printf("s> Error en recepcion..\n");
        close(sc);
    }
    //Registro
    else if (strcmp(op, "REGISTER") == 0)
    {
        /* Leemos el usuario del buffer */
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(user, buffer);
        res = exist(users, user);
        if (res == 1)
        {
            printf("s> REGISTER %s FAIL\n", user);
        }
        else
        {
            res = add_username(&users, user, 0, "DESCONECTADO", 0, 0);
            printf("s> REGISTER %s OK\n", user);

            clnt = clnt_create(IP_RPC, COMUNICACION, COMUNICACIONVER, "tcp");
            if (clnt == NULL)
            {
                clnt_pcreateerror(IP_RPC);
                exit(1);
            }
            strcpy(user_registered.username, user);
            retval = add_register_rpc_1(user_registered, &response, clnt);
            if (retval != RPC_SUCCESS)
            {
                clnt_perror(clnt, "call failed:");
            }
            clnt_destroy(clnt);
        }
        sprintf(buffer, "%d", res);
        sendMessage(sc, buffer, sizeof(buffer) + 1);
        close(sc);
    }
    else if (strcmp(op, "UNREGISTER") == 0)
    {
        /* Leemos el usuario del buffer */
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(user, buffer);
        if (exist(users, user) == 1)
        {
            res = delete_username(&users, user);
            printf("s> UNREGISTER %s OK\n", user);
            clnt = clnt_create(IP_RPC, COMUNICACION, COMUNICACIONVER, "tcp");
            if (clnt == NULL)
            {
                clnt_pcreateerror(IP_RPC);
                exit(1);
            }
            strcpy(user_registered.username, user);
            retval = delete_register_rpc_1(user_registered, &response, clnt);
            if (retval != RPC_SUCCESS)
            {
                clnt_perror(clnt, "call failed:");
            }
            clnt_destroy(clnt);
        }
        else
        {
            res = 1;
            printf("s> UNREGISTER %s FAIL\n", user);
        }
        sprintf(buffer, "%d", res);
        sendMessage(sc, buffer, sizeof(buffer) + 1);
        close(sc);
    }
    else if (strcmp(op, "CONNECT") == 0)

    {
        /* Leemos el usuario del buffer */
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(user, buffer);

        err = readLine(sc, buffer, MAXSIZE);
        strcpy(port, buffer);
        if (exist(users, user) == 1)
        {
            if (get_user(&users, user)->status == 1)
            {
                res = 2;
                printf("s> CONNECT %s FAIL\n", user);
            }
            else
            {
                res = modify_value(&users, user, 1, inet_ntoa(client_addr.sin_addr), atoi(port));
                strcpy(user2, user);

                while (num_messages(get_user(&users, user2)->l_message, user) != 0)
                {
                    strcpy(user, get_message(get_user(&users, user2)->l_message)->sender);
                    if (get_user(&users, user)->status == 1)
                    {
                        strcpy(message, get_message(get_user(&users, user2)->l_message)->message);
                        printf("%s", message);

                        if ((soc_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                        {
                            printf("s> SERVER: Error en el socket\n");
                            return;
                        }

                        //Cojo la IP y el puerto del receptor
                        strcpy(IP, get_user(&users, user2)->IP);

                        sprintf(port, "%d", get_user(&users, user2)->port);

                        struct sockaddr_in client;

                        bzero((char *)&client, sizeof(client));

                        client.sin_family = AF_INET;

                        struct in_addr address;

                        inet_aton(IP, &address);

                        hp = gethostbyaddr(&address, sizeof(address), AF_INET);

                        memcpy(&(client.sin_addr), hp->h_addr, hp->h_length);

                        client.sin_port = htons(atoi(port));
                        // Me conecto al puerto de escucha del cliente receptor
                        int c = connect(soc_desc, (struct sockaddr *)&client, sizeof(client));
                        if (c == -1)
                        {
                            printf("Error al conectar al puerto de escucha\n");
                            close(soc_desc);
                        }
                        else
                        {
                            // Borro el mensaje si el usuario receptor esta conectado
                            int mess_id = get_message(get_user(&users, user2)->l_message)->mid;
                            delete_message(&users, user2, mess_id);
                            //Envio de send_message
                            sendMessage(soc_desc, "SEND_MESSAGE", strlen("SEND_MESSAGE") + 1);
                            //Envio del usuario que envia el mensaje
                            sendMessage(soc_desc, user, strlen(user) + 1);
                            //Envio del id del mensaje
                            sprintf(buffer, "%d", mess_id);
                            sendMessage(soc_desc, buffer, strlen(buffer) + 1);
                            //Envio del mensaje
                            sendMessage(soc_desc, message, strlen(message) + 1);

                            printf("s> SEND MESSAGE %s FROM %s TO %s\n", buffer, user, user2);

                            close(soc_desc);
                            // Parte del ACK
                            if ((soc_dest = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                            {
                                printf("s> SERVER: Error en el socket");
                                return;
                            }
                            strcpy(IP, get_user(&users, user)->IP);
                            sprintf(port, "%d", get_user(&users, user)->port);
                            struct sockaddr_in client2;
                            bzero((char *)&client2, sizeof(client2));
                            client2.sin_family = AF_INET;
                            struct in_addr address;
                            inet_aton(IP, &address);
                            hp = gethostbyaddr(&address, sizeof(address), AF_INET);
                            memcpy(&(client2.sin_addr), hp->h_addr, hp->h_length);
                            client2.sin_port = htons(atoi(port));
                            // Me conecto al puerto de escucha del cliente remitente
                            int c = connect(soc_dest, (struct sockaddr *)&client2, sizeof(client2));
                            if (c == -1)
                            {
                                printf("s> Error al conectar al puerto de escucha\n");
                                close(soc_dest);
                            }
                            else
                            {
                                //Envio del send message ack
                                sendMessage(soc_dest, "SEND_MESS_ACK", strlen("SEND_MESS_ACK") + 1);
                                // Envio del id del ack
                                sendMessage(soc_dest, buffer, strlen(buffer) + 1);
                            }
                            close(soc_dest);
                        }
                    }
                }
                printf("s> CONNECT %s OK\n", user2);
            }
        }
        else
        {
            res = 1;
            printf("s> CONNECT %s FAIL\n", user2);
        }
        sprintf(buffer, "%d", res);
        sendMessage(sc, buffer, sizeof(buffer) + 1);
        close(sc);
    }

    else if (strcmp(op, "DISCONNECT") == 0)
    {
        /* Leemos el usuario del buffer */
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(user, buffer);
        if (exist(users, user) == 1)
        {
            if (get_user(&users, user)->status == 1 && strcmp(get_user(&users, user)->IP, inet_ntoa(client_addr.sin_addr)) == 0)
            {
                res = modify_value(&users, user, 0, "", 0);

                printf("s> DISCONNECT %s OK\n", user);
            }
            else
            {
                res = 2;
                printf("s> DISCONNECT %s FAIL\n", user);
            }
        }
        else
        {
            res = 1;
            printf("s> DISCONNECT %s FAIL\n", user);
        }
        sprintf(buffer, "%d", res);
        sendMessage(sc, buffer, sizeof(buffer) + 1);
        close(sc);
    }
    else if (strcmp(op, "SEND") == 0)
    {
        // Guardo el usuario que envia el mensaje
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(user, buffer);

        if (err == -1)
        {
            printf("s> Error en recepcion..\n");
            close(sc);
        }
        // Guardo el receptor
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(user2, buffer);

        if (err == -1)
        {
            printf("s> Error en recepcion..\n");
            close(sc);
        }
        // Guardo el mensaje
        err = readLine(sc, buffer, MAXSIZE);
        strcpy(message, buffer);

        if (err == -1)
        {
            printf("s> Error en recepcion..\n");
            close(sc);
        }
        if (exist(users, user) == 1 && exist(users, user2) == 1)
        {
            res = add_messsage(&users, user2, user, message);
            printList(users);

            if (get_user(&users, user2)->status == 1)
            {
                if ((soc_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                    printf("s> SERVER: Error en el socket\n");
                    return;
                }
                //Cojo la IP y el puerto del receptor
                strcpy(IP, get_user(&users, user2)->IP);
                sprintf(port, "%d", get_user(&users, user2)->port);

                struct sockaddr_in client;
                bzero((char *)&client, sizeof(client));
                client.sin_family = AF_INET;
                struct in_addr address;
                inet_aton(IP, &address);
                hp = gethostbyaddr(&address, sizeof(address), AF_INET);
                memcpy(&(client.sin_addr), hp->h_addr, hp->h_length);
                client.sin_port = htons(atoi(port));
                // Me conecto al puerto de escucha del cliente receptor
                int c = connect(soc_desc, (struct sockaddr *)&client, sizeof(client));
                if (c == -1)
                {
                    printf("s> Error al conectar al puerto de escucha\n");
                    close(soc_desc);
                }
                else
                {
                    // Borro el mensaje si el usuario receptor esta conectado
                    delete_message(&users, user2, get_user(&users, user2)->mid);
                    //Envio de send_message
                    sendMessage(soc_desc, "SEND_MESSAGE", strlen("SEND_MESSAGE") + 1);
                    //Envio del usuario que envia el mensaje
                    sendMessage(soc_desc, user, strlen(user) + 1);
                    //Envio del id del mensaje
                    sprintf(buffer, "%d", get_user(&users, user2)->mid);
                    sendMessage(soc_desc, buffer, strlen(buffer) + 1);
                    //Envio del mensaje
                    sendMessage(soc_desc, message, strlen(message) + 1);

                    printf("s> SEND MESSAGE %s FROM %s TO %s\n", buffer, user, user2);

                    close(soc_desc);
                    // Parte del ACK
                    if ((soc_dest = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                    {
                        printf("s> SERVER: Error en el socket");
                        return;
                    }
                    strcpy(IP, get_user(&users, user)->IP);
                    sprintf(port, "%d", get_user(&users, user)->port);

                    struct sockaddr_in client2;
                    bzero((char *)&client2, sizeof(client2));
                    client2.sin_family = AF_INET;
                    struct in_addr address;
                    inet_aton(IP, &address);
                    hp = gethostbyaddr(&address, sizeof(address), AF_INET);
                    memcpy(&(client2.sin_addr), hp->h_addr, hp->h_length);
                    client2.sin_port = htons(atoi(port));
                    // Me conecto al puerto de escucha del cliente remitente
                    int c = connect(soc_dest, (struct sockaddr *)&client2, sizeof(client2));
                    if (c == -1)
                    {
                        printf("s> Error al conectar al puerto de escucha\n");
                        close(soc_dest);
                    }
                    else
                    {
                        if (res == 0)
                        {
                            //Envio del send message ack
                            sendMessage(soc_dest, "SEND_MESS_ACK", strlen("SEND_MESS_ACK") + 1);
                            // Envio del id del ack
                            sendMessage(soc_dest, buffer, strlen(buffer) + 1);
                        }
                    }
                    close(soc_dest);
                }
            }
            else
            {
                printf("s> SEND MESSAGE %s FROM %s TO %s STORED\n", buffer, user, user2);
            }
        }
        else if(exist(users, user) != 1 || exist(users, user2) != 1)
        {
            res = 1;
        } else{
            res = 2;
        }
        

        
        sprintf(buffer, "%d", res);
        sendMessage(sc, buffer, strlen(buffer) + 1);
 
        sprintf(buffer, "%d", get_user(&users, user2)->mid);
        sendMessage(sc, buffer, strlen(buffer) + 1);
        close(sc);
    }
}
