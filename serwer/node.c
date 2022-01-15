#include "node.h"

struct node *filesHead = NULL;

void insertFirst(char* data){
    struct node *link = (struct node*) malloc(sizeof(struct node));
    char *copyOfData = (char*) malloc (sizeof(char)*20);
    memcpy(copyOfData, data, sizeof(char)*20);
    //printf("1. %s\n", data);
    printf("Dodano pokoj %s\n", copyOfData);
    link->nazwa = copyOfData;
    link->ptr = filesHead;

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

        if(filesCurrent == filesHead) {
            filesHead = filesHead->ptr;
        }else{
            previous->ptr = filesCurrent->ptr;
        }
        return filesCurrent;
    }
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
        write(fd, current->nazwa, 20);
        current= current->ptr;
    }
    write(fd, "", 20);
}