#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct node{
    char* nazwa;
    int obecni[50];
    struct node* ptr;
};


void insertFirst(char data[], int fd);
struct node* deleteNode(char* data);
int length();
void printList();
void sendList(int fd);
struct node* exitNode(char* data, int fd);
int openNode(char * data, int fd);
int checkAndDelete(char* data);


