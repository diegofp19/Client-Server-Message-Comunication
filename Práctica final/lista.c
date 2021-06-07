#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"


int add_username(userList *l, char *username, int status, char *IP, int port, int mid)
{
    struct userNode *ptr;
    ptr = (struct userNode *)malloc(sizeof(struct userNode));
    if (ptr == NULL || exist(*l, username) == 1)
        return -1;
    if (*l == NULL)
    {
        strcpy(ptr->username, username);
        ptr->status = status;
        strcpy(ptr->IP, IP);
        ptr->port = port;
        ptr->mid = mid;
        ptr->l_message = NULL;
        ptr->next = NULL;
        *l = ptr;
    }
    else
    {
        strcpy(ptr->username, username);
        ptr->status = status;
        strcpy(ptr->IP, IP);
        ptr->port = port;
        ptr->mid = mid;
        ptr->l_message = NULL;
        ptr->next = *l;
        *l = ptr;
    }
    return 0;
}

int add_register(registerList *l, char *username)
{
    struct registerNode *ptr;
    ptr = (struct registerNode *)malloc(sizeof(struct registerNode));
    if (ptr == NULL || existRegister(*l, username) == 1)
        return -1;
    if (*l == NULL)
    {
        strcpy(ptr->username, username);
        ptr->next = NULL;
        *l = ptr;
    }
    else
    {
        strcpy(ptr->username, username);
        ptr->next = *l;
        *l = ptr;
    }
    printf("REGISTER RPC OK \n");
    printListRegister(*l);
    return 0;
}

int add_messsage(userList *l, char *receptor, char* sender, char *message)
{
    
    struct userNode *dest;
    struct userNode *transmitter;

    transmitter = get_user(l, sender);

    dest = get_user(l, receptor);

    if(transmitter == NULL || receptor == NULL){
        return 1;
    }

    struct messageNode *message_to_send;

    message_to_send = (struct messageNode *)malloc(sizeof(struct messageNode));
    
    if(message_to_send == NULL){
        return 2;
    }
    
    if( dest->l_message == NULL){
        
        strcpy(message_to_send->message, message);
        strcpy(message_to_send->sender, sender);
        if(dest->mid+1 > sizeof(unsigned int)){
            dest->mid = 1;
        }else{
            dest->mid = dest->mid+1;
        }
        
        
        message_to_send->mid = dest->mid;
        
        message_to_send->next = NULL;
        
        dest->l_message = message_to_send;
        
    }else{
        
        strcpy(message_to_send->message, message);
        strcpy(message_to_send->sender, sender);
        dest->mid = dest->mid+1;
        message_to_send->mid = dest->mid;
        message_to_send->next = dest->l_message;
        dest->l_message = message_to_send; 
    }


    return 0;

    
    

}




int modify_value(userList *l, char *username, int status, char *IP, int port)
{
    userList aux;
    aux = *l;
    while (aux != NULL)
    {
        if (strcmp(aux->username, username) == 0)
        {
            aux->status = status;
            strcpy(aux->IP, IP);
            aux->port = port;
            return 0;
        }
        else
        {
            aux = aux->next;
        }
    }
    return -1;
}

int printList(userList l)
{
    userList aux;
    aux = l;
    while (aux != NULL)
    {
        printf("username= %s status= %d IP= %s port= %d mid= %d\n", aux->username, aux->status, aux->IP, aux->port, aux->mid);
        aux = aux->next;
    }
    return 0;
}

void printListRegister(registerList l)
{
    registerList aux;
    aux = l;
    while (aux != NULL)
    {
        printf("username= %s\n", aux->username);
        aux = aux->next;
    }
}

void printMessage(messageList l)
{   
    messageList aux;
    aux = l;
    while (aux != NULL)
    {
        printf("menssage= %s User= %s ID= %d\n", aux->message, aux->sender, aux->mid );
        aux = aux->next;
    }
    
}

struct userNode *get_user(userList *l, char *username){
    userList aux;
    aux = *l;
    while (aux != NULL)
    {
        if (strcmp(aux->username, username) == 0)
        {
            return aux;
        }
        else
        {
            aux = aux->next;
        }
    }
    return NULL;
}

// Obtengo la estructura del mensaje requerido
struct messageNode *get_message(messageList l){
    messageList aux;
    aux = l;
    while(aux->next != NULL){
        aux = aux->next;
    }
    return aux;
}

int num_messages(messageList l, char *username)
{

    messageList aux;
    aux = l;
    if (aux == NULL)
        return 0;
    int contador = 0;
    while (aux != NULL)
    {
        aux = aux->next;
        contador++;
    }
    return contador;
}

int delete_username(userList *l, char *username)
{
    userList aux, back;
    if (*l == NULL)
        return -1;
    if (strcmp(username, (*l)->username) == 0)
    {
        aux = *l;
        *l = (*l)->next;
        free(aux);
        return 0;
    }

    aux = *l;
    back = *l;
    while (aux != NULL)
    {
        if (strcmp(aux->username, username) == 0)
        {
            back->next = aux->next;
            free(aux);
            return 0;
        }
        else
        {
            back = aux;
            aux = aux->next;
        }
    }
    return -1;
}

int delete_register(registerList *l, char *username)
{
    registerList aux, back;
    if (*l == NULL)
        return -1;
    if (strcmp(username, (*l)->username) == 0)
    {
        aux = *l;
        *l = (*l)->next;
        free(aux);
        printf("UNREGISTER RPC OK \n");
        printListRegister(*l);
        return 0;
    }

    aux = *l;
    back = *l;
    while (aux != NULL)
    {
        if (strcmp(aux->username, username) == 0)
        {
            back->next = aux->next;
            free(aux);
            printf("UNREGISTER RPC OK \n");
            printListRegister(*l);
            return 0;
        }
        else
        {
            back = aux;
            aux = aux->next;
        }
    }
        
    return -1;
}

int exist(userList l, char *username)
{
    userList aux;
    aux = l;
    while (aux != NULL)
    {
        if (strcmp(aux->username, username) == 0)
        {
            return 1;
        }
        else
        {
            aux = aux->next;
        }
    }
    return 0;
}

int existRegister(registerList l, char *username)
{
    registerList aux;
    aux = l;
    while (aux != NULL)
    {
        if (strcmp(aux->username, username) == 0)
        {
            return 1;
        }
        else
        {
            aux = aux->next;
        }
    }
    return 0;
}

int delete_message(userList *l, char *username, int mid)
{
    struct userNode *usuario;

    usuario = get_user(l, username);

    messageList aux, back;
    aux = usuario->l_message;

    if (usuario->l_message == NULL)
        return -1;
    if (mid == (usuario->l_message)->mid)
    {
        aux = usuario->l_message;
        usuario->l_message = (usuario->l_message)->next;
        free(aux);
        return 0;
    }

    aux = usuario->l_message;
    back = usuario->l_message;
    while (aux != NULL)
    {
        if (aux->mid==mid)
        {
            back->next = aux->next;
            free(aux);
            return 0;
        }
        else
        {
            back = aux;
            aux = aux->next;
        }
    }
    return -1;
}