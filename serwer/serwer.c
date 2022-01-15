#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "node.h"

void bWrite(int fd, char *buf, int n){
    int i = 0;
    while(i < n){
        int j = write(fd, buf+i, n-i);
        i += j;
    }
}

void bRead(int fd, char *buf, int n){
    int i = 0;
    while(i < n){
        int j = read(fd, buf+i, n-i);
        i += j;
    }
}

void createRoom(char* filename){
    int fd;
    if((fd = creat(filename, 0777)) < 0){
        printf("Blad przy tworzeniu pokoju %s\n", filename);
    }
    close(fd);
}

void closeRoom(char* filename){
    if(remove(filename) < 0){
        printf("Blad przy usuwaniu pokoju %s\n", filename);
    }
}

// Function to swap two numbers
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    // consider the absolute value of the number
    int n = abs(value);
 
    int i = 0;
    while (n)
    {
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

int main(int argc, char** argv){
    socklen_t slt;
    int sfd, cfd, fdmax, fda, rc, i, on = 1;
    struct sockaddr_in saddr, caddr;
    static struct timeval timeout;
    fd_set mask, rmask, wmask;

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(1234);
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    listen(sfd, 10);
    FD_ZERO(&mask);
    FD_ZERO(&rmask);
    FD_ZERO(&wmask);
    fdmax = sfd;

    //Tworzenie pokoju
    char* nzw = "mainRoom.txt";
    createRoom(nzw);
    insertFirst(nzw);

    while(1){
        FD_SET(sfd, &rmask);
        wmask = mask;
        timeout.tv_sec = 5 * 60;
        timeout.tv_usec = 0;
        rc = select(fdmax + 1, &rmask, &wmask, (fd_set*)0, &timeout);
        if (rc == 0) {
            printf("timed out \n");
            continue;
        }
        fda = rc;
        if(FD_ISSET(sfd, &rmask)){
            fda -= 1;
            slt = sizeof(caddr);
            cfd = accept(sfd, (struct sockaddr*)&caddr, &slt);
            printf("new connection: %s \n", inet_ntoa((struct in_addr)caddr.sin_addr));
            FD_SET(cfd, &mask);
            if(cfd > fdmax) fdmax = cfd;
        }
        for(i = sfd + 1; i <= fdmax && fda > 0; i++)
            if(FD_ISSET(i, &wmask)) {
                fda -= 1;
                char command[8];
                char roomName[20];
                char userName[20];
                char message[200];
                char index[20];
                bRead(i, command, 100);
                printf("recaived message: %s \n", command);
                char nazwa[50] = {0};
                /*
                    "newMsg" - serwer wysyla informacje ze odebral wiadomosc, 
                    pozniej wczytuje nazwe pokoju,
                    wczytuje nazwe uzytkownika,
                    a po nim wiadomosc
                    Nastepnie zapisuje wiadomosc do pliku
                */
                if(!(strcmp(command, "newMsg"))){
                    strcpy(nazwa, "Dodano twoja wiadomosc\n");
                    bWrite(i, nazwa, 100);

                    //Serwer odczytuje informacje
                    bRead(i, roomName, 20);
                    printf("\tNazwa pokoju: %s\n", roomName);
                    bRead(i, userName, 20);
                    printf("\tNazwa uzytkownika: %s\n", userName);
                    bRead(i, message, 200);
                    printf("\tWiadomosc: %s\n", message);

                    //serwer otwiera plik i zapisuje sformatowany wpis
                    FILE* mainRoom = fopen(roomName, "a");
                    fprintf(mainRoom,"%s: %s\n", userName, message);
                    fclose(mainRoom);
                    memset(message, '\0', sizeof(char)* 200);
                } 
                /*
                    "newRoom" - Serwer wysyla wiadomosc ze utworzono pokoj,
                    nastepnie wczytuje nazwe pokoju
                    tworzy pokoj i dodaje go do listy pokoi
                */
                else if(!(strcmp(command, "newRoom"))){
                    strcpy(nazwa, "Utworzono nowy pokoj\n");
                    bWrite(i, nazwa, 100);

                    //serwer wczytuje nazwe
                    bRead(i, roomName, 20);
                    printf("recaived room name: %s \n", roomName);

                    //serwer wpisuje nazwe pokoju do tablicy i tworzy pokoj
                    insertFirst(roomName);
                    createRoom(roomName);
                    printList();
                } 

                /*
                    "clRoom" - uzytkownik zamyka pokoj, i podaje jaki pokoj zamknac.
                */
                else if(!(strcmp(command, "clRoom"))){
                    
                    strcpy(nazwa, "Zamykanie pokoju\n");
                    bWrite(i, nazwa, 100);

                    //serwer wczytuje nazwe
                    bRead(i, roomName, 20);
                    printf("recaived room name: %s \n", roomName);

                    //serwer usuwa nazwe pokoju z tablicy i usuwa pokoj
                    deleteNode(roomName);
                    closeRoom(roomName);
                    printList();
                } 

                /*
                    "refresh" - uzytkownik pyta czy są nowe wiadomosci.
                    klient podaje informacje o jaki pokoj pyta
                    podaje miejsce w ktorym ostatnio byl w pliku
                    serwer odczytuje wszystkie dane do konca pliku i wysyla klientowi
                    pozniej wysyla pustego stringa na znak konca i nowa pozycje
                */
                else if(!(strcmp(command, "refresh")))
                {
                    strcpy(nazwa, "Sprawdzanie czy sa nowe wiadomosci\n");
                    bWrite(i, nazwa, 100);

                    bRead(i, roomName, 20);
                    printf("recaived roomname: %s \n", roomName);

                    bRead(i, index, 20);
                    int ile = atoi(index);
                    printf("recaived number: %d\n", ile);

                    int source = open(roomName, O_RDONLY);
                    if(source < 0){
                        printf("Blad przy otwieraniu pokoju %s\n", roomName);
                    }else{
                        printf("Udalo sie otworzyc pokoj %s w deskryptorze %d\n", roomName, source);
                    }
                    int end = lseek(source, 0, SEEK_END);
                    printf("Koniec pliku: %d\n", end);
                    lseek(source, ile, SEEK_SET);
                    for(; ile < end;){
                        int n = read(source, message, 200);
                        printf("%s", message);
                        bWrite(i, message, 200);
                        memset(message, '\0', sizeof(char)* 200);
                        ile += n;
                    }
                    bWrite(i, message, 200);

                    itoa(ile, index, 10);
                    bWrite(i, index, 20);
                    close(source);
                }

                /*
                    "listPok" - uzytkownik prosi o liste pokoi
                    serwer wysyja liste pokoi 
                    nastepnie pustego stringa
                */
                else if(!(strcmp(command, "listPok"))){
                    strcpy(nazwa, "Sprawdzanie listy pokoi \n");
                    bWrite(i, nazwa, 100);

                    sendList(i);
                }
                /*
                    "exit" - serwer wysyla wiadomosc o potwierdzeniu konczenia polaczenia
                    wyswietla informacje w konsoli
                    konczy polaczenie
                */
                else if(!(strcmp(command, "exit"))){
                    strcpy(nazwa, "Konczenie polaczenia\n");
                    bWrite(i, nazwa, 100);

                    printf("zamykanie polaczenia z %d\n", i);
                    close(i);
                    FD_CLR(i, &mask);
                    if(i == fdmax)
                        while(fdmax > sfd && !FD_ISSET(fdmax,&mask))
                            fdmax -= 1;
                } else {
                    strcpy(nazwa, "Error");
                    bWrite(i, nazwa, 100);
                }
                
            }
    }
    close(sfd);
    return EXIT_SUCCESS;
}