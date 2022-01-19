#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include "node.h"
/*
    Funkcja która usuwa wszystkie utworzone pliki w razie zamknięcia serwera
*/
void sig_handler(int sig){
    if(sig == SIGINT){
        deleteAll();
        abort();
    }
}

/*
    Ulepszona funkcja Write, pozwala na wysyłanie wiadomosci nawet jesli przepustowosc jest bardzo mala
*/
void bWrite(int fd, char *buf, int n){
    int i = 0;
    while(i < n){
        int j = write(fd, buf+i, n-i);
        i += j;
    }
}

/*
    Ulepszona funkcja Read, pozwala na odbieranie wiadomosci kiedy przepustowosc jest bardzo mala
*/
void bRead(int fd, char *buf, int n){
    int i = 0;
    while(i < n){
        int j = read(fd, buf+i, n-i);
        i += j;
    }
}

/*
    Funkcja do tworzenia nowych plikow tekstowych
*/
void createRoom(char* filename){
    int fd;
    if((fd = creat(filename, 0777)) < 0){
        printf("Blad przy tworzeniu pokoju %s\n", filename);
    }
    close(fd);
}

/*
    Funkcja do usuwania plików tekstowych
*/
void closeRoom(char* filename){
    if(remove(filename) < 0){
        printf("Blad przy usuwaniu pokoju %s\n", filename);
    }
}

/*
    Funkcja do zamiany dwóch znaków - potrzebna do funkcji ITOA
*/
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
/*
    Funkcja do odwracania znaków w buforze - potrzebna do funkcji ITOA
*/
char* reverse(char *buffer, int i, int j)
{
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
 
/*
    Funkcja ITOA, potrzebna do zamiany indeksu na napis.
*/
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

/*
    Struktura przechowująca deskryptor klienta i informacje o połączeniu
*/
struct cln {
	int cfd;
	struct sockaddr_in caddr;
};

/*
    Główna funkcja która wykonuje się w nowych wątkach, po odebraniu połączenia
*/
void* cthread(void* arg) {
	struct cln* c = (struct cln*)arg;
	printf("New connection: %s\n", inet_ntoa((struct in_addr)c->caddr.sin_addr));
	while(1){
		
                char command[8];
                char roomName[20];
                char userName[20];
                char message[200];
                char index[20];
                bRead(c->cfd, command, 200);
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
                    bWrite(c->cfd, nazwa, 200);

                    //Serwer odczytuje informacje
                    bRead(c->cfd, roomName, 200);
                    printf("\tNazwa pokoju: %s\n", roomName);
                    bRead(c->cfd, userName, 200);
                    printf("\tNazwa uzytkownika: %s\n", userName);
                    bRead(c->cfd, message, 200);
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
                    bWrite(c->cfd, nazwa, 200);

                    //serwer wczytuje nazwe
                    bRead(c->cfd, roomName, 200);
                    printf("recaived room name: %s \n", roomName);

                    //serwer wpisuje nazwe pokoju do tablicy i tworzy pokoj
                    insertFirst(roomName, c->cfd);
                    createRoom(roomName);
                    printList();
                } 

                /*
                    "onRoom" - otwiera pokoj, serwer wysyla wiadomosc ze uzytkownik otworzyl
                    pokoj, i wpisuje jego indeks do tablicy.
                    Na koniec wysyla potwierdzenie ze sie udalo lub nie udalo.
                */

                else if(!(strcmp(command, "onRoom"))){
                    
                    strcpy(nazwa, "Otwieranie pokoju\n");
                    bWrite(c->cfd, nazwa, 200);

                    //serwer wczytuje nazwe
                    bRead(c->cfd, roomName, 200);
                    printf("recaived room name: %s \n", roomName);

                    //serwer dodaje do tablicy deskyptor uzytkownika
                    int r = openNode(roomName, c->cfd);

                    if(r > 0){
                        strcpy(nazwa, "Tak");
                    }else{
                        strcpy(nazwa, "Nie");
                    }
                    bWrite(c->cfd, nazwa, 200);
                }

                /*
                    "clRoom" - uzytkownik zamyka pokoj, i podaje jaki pokoj zamknac.
                */
                else if(!(strcmp(command, "clRoom"))){
                    
                    strcpy(nazwa, "Zamykanie pokoju\n");
                    bWrite(c->cfd, nazwa, 200);

                    //serwer wczytuje nazwe
                    bRead(c->cfd, roomName, 200);
                    printf("recaived room name: %s \n", roomName);

                    //serwer usuwa nazwe pokoju z tablicy i usuwa pokoj
                    exitNode(roomName, c->cfd);
                    int r = checkAndDelete(roomName);
                    if(r > 0) closeRoom(roomName);
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
                    bWrite(c->cfd, nazwa, 200);

                    bRead(c->cfd, roomName, 200);
                    printf("recaived roomname: %s \n", roomName);

                    bRead(c->cfd, index, 200);
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
                        bWrite(c->cfd, message, 200);
                        memset(message, '\0', sizeof(char)* 200);
                        ile += n;
                    }
                    bWrite(c->cfd, message, 200);

                    itoa(ile, index, 10);
                    bWrite(c->cfd, index, 200);
                    close(source);
                }

                /*
                    "listPok" - uzytkownik prosi o liste pokoi
                    serwer wysyja liste pokoi 
                    nastepnie pustego stringa
                */
                else if(!(strcmp(command, "listPok"))){
                    strcpy(nazwa, "Sprawdzanie listy pokoi \n");
                    bWrite(c->cfd, nazwa, 200);

                    sendList(c->cfd);
                }
                /*
                    "exit" - serwer wysyla wiadomosc o potwierdzeniu konczenia polaczenia
                    wyswietla informacje w konsoli
                    konczy polaczenie
                */
                else if(!(strcmp(command, "exit"))){
                    strcpy(nazwa, "Konczenie polaczenia\n");
                    bWrite(c->cfd, nazwa, 200);

                    printf("zamykanie polaczenia z %d\n", c->cfd);
					close(c->cfd);
					free(c);
                    
                } else {
                    strcpy(nazwa, "Error");
                    bWrite(c->cfd, nazwa, 200);
                }
	}
	return EXIT_SUCCESS;
}


int main(int argc, char** argv){
	
	pthread_t tid;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(1234);
	saddr.sin_addr.s_addr = INADDR_ANY;

	bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
	listen(fd, 10);

    //Tworzenie pokoju
    char* nzw = "MainRoom";
    createRoom(nzw);
    insertFirst(nzw, 1);

	socklen_t s ;

    signal(SIGINT, sig_handler);

	while(1){
		struct cln* c = malloc(sizeof(struct cln));
		s = sizeof(c->caddr);
		c->cfd = accept(fd, (struct sockaddr *)&c->caddr, &s);
		pthread_create(&tid, NULL, cthread, c);
		pthread_detach(tid);
        
	}
	close(fd);
	return EXIT_SUCCESS;


}
