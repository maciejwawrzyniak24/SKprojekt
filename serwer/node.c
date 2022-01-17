#include "node.h"

struct node *filesHead = NULL;

void insertFirst(char* data, int fd){
    struct node *link = (struct node*) malloc(sizeof(struct node));
    char *copyOfData = (char*) malloc (sizeof(char)*20);
    memcpy(copyOfData, data, sizeof(char)*20);
    //printf("1. %s\n", data);
    printf("Dodano pokoj %s\n", copyOfData);
    link->nazwa = copyOfData;
    link->ptr = filesHead;
    memset(link->obecni, 0, 50);
    link->obecni[0] = fd;

    filesHead = link;
}

struct node* deleteNode(char* data){
    struct node* filesCurrent = filesHead;
    struct node* previous = NULL;

    if(filesHead == NULL){
        return NULL;
    }

    while(strcmp(filesCurrent->nazwa, data)){
        if(filesCurrent->ptr == NULL){
            return NULL;
        }else{
            previous = filesCurrent;
            filesCurrent = filesCurrent->ptr;
        }
    }
    if(filesCurrent == filesHead) {
        filesHead = filesHead->ptr;
    }else{
        previous->ptr = filesCurrent->ptr;
    }
    return filesCurrent;
    
}

int length(){
    int length = 0;
    struct node *current;

    for(current = filesHead; current != NULL; current = current->ptr){
        length++;
    }
    return length;
}

void printList(){
    struct node* current = filesHead;
    printf("\n[ ");

    while(current != NULL){
        printf("%s ", current->nazwa);
        current = current->ptr;
    }
    printf("]\n");
}

void sendList(int fd){
    struct node* current = filesHead;
    while(current != NULL){
        write(fd, current->nazwa, 200);
        current= current->ptr;
    }
    write(fd, "", 200);
}

/*
    Sprawdza wezly, jesli udalo sie znalezc to sprawdza czy jest taki z dana wartoscia, jesli jest to 
    zmienia go na 0
*/

struct node* exitNode(char* data, int fd){
    struct node* filesCurrent = filesHead;
    struct node* previous = NULL;

    if(filesHead == NULL){
        return NULL;
    }

    while(strcmp(filesCurrent->nazwa, data)){
        if(filesCurrent->ptr == NULL){
            return NULL;
        }else{
            previous = filesCurrent;
            filesCurrent = filesCurrent->ptr;
        }
    }
    for(int i = 0; i < 50; i++){
        if(filesCurrent->obecni[i] == fd){
            filesCurrent->obecni[i] = 0;
        }
    }
    return filesCurrent;
    
}

/*
    Sprawdza czy istnieje wezel o danej nazwie, jesli tak to sprawdza tablice obecni czy sa wolne miejsca
    jesli sa to wpisuje w pierwsze wolne miejsce. Przy powodzeniu wysyla 1, przy nie udanym wpisaniu
    wysyla 0 a jesli nie udalo sie znalezc wysyla -1
*/

int openNode(char* data, int fd){
    struct node* filesCurrent = filesHead;
    struct node* previous = NULL;

    if(filesHead == NULL){
        return -1;
    }

    while(strcmp(filesCurrent->nazwa, data)){
        if(filesCurrent->ptr == NULL){
            return -1;
        }else{
            previous = filesCurrent;
            filesCurrent = filesCurrent->ptr;
        }
    }
    int d = 0;
    for(int i = 0; i < 50; i++){
        if(filesCurrent->obecni[i] == 0){
            filesCurrent->obecni[i] = fd;
            d = 1;
            break;
        }
    }
    return d;
}

/*
    sprawdza wezly, jesli jakis wezel ma ta sama nazwe to sprawdza tablice obecni, czy jest cala
    wyzerowana, jesli tak to wysyla 1, jesli nie to wysyla 0 a jesli nie znalazlo nazwy to -1
*/

int checkAndDelete(char* data){
    struct node* filesCurrent = filesHead;
    struct node* previous = NULL;

    if(filesHead == NULL){
        return -1;
    }

    while(strcmp(filesCurrent->nazwa, data)){
        if(filesCurrent->ptr == NULL){
            return -1;
        }else{
            previous = filesCurrent;
            filesCurrent = filesCurrent->ptr;
        }
    }
    int d = 1;
    for(int i = 0; i < 50; i++){
        if(filesCurrent->obecni[i] != 0){
            d = 0;
            break;
        }
    }
    if(d > 0){
        if(filesCurrent == filesHead) {
            filesHead = filesHead->ptr;
        }else{
            previous->ptr = filesCurrent->ptr;
        }
    }
    return d;
}