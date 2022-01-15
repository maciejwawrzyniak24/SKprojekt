#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct node{
    char* nazwa;
    struct node* ptr;
};


void insertFirst(char data[]);
struct node* deleteNode(char* data);
int length();
void printList();
void sendList(int fd);


