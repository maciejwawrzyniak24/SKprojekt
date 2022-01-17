
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    char hostname[] = "192.168.8.121";
    WORD WRequiredVersion;
    WSADATA WData;
    SOCKET SSocket;
    int nConnect;
    int nBytes;
    struct sockaddr_in stServerAddr;
    struct hostent* lpstServerEnt;
    char cbBuf[BUF_SIZE];

    /*if (argc != 3)
    {
        fprintf(stderr, "Usage: %s server_name port_number\n", argv[0]);
        exit(1);
    }*/

    /* initialize winsock */
    WRequiredVersion = MAKEWORD(2, 0);
    if (WSAStartup(WRequiredVersion, &WData) != 0) {
        fprintf(stderr, "WSAStartup failed!");
        exit(1);
    }

    /* look up server's IP address */
    lpstServerEnt = gethostbyname(hostname);
    if (!lpstServerEnt)
    {
        fprintf(stderr, "%s: Can't get the server's IP address.\n", argv[0]);
        exit(1);
    }
    /* create a socket */
    SSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* server info */
    memset(&stServerAddr, 0, sizeof(struct sockaddr));
    stServerAddr.sin_family = AF_INET;
    memcpy(&stServerAddr.sin_addr.s_addr, lpstServerEnt->h_addr, lpstServerEnt->h_length);
    stServerAddr.sin_port = htons(1234);

    /* connect to the server */
    nConnect = connect(SSocket, (struct sockaddr*)&stServerAddr, sizeof(struct sockaddr));
    if (nConnect < 0)
    {
        fprintf(stderr, "%s: Can't connect to the server .\n", argv[0]);
        exit(1);
    }

    char nazwaUzytkownika[20];
    printf("Podaj nazwe uzytkownika: \n");
    scanf_s("%s", nazwaUzytkownika, (unsigned)_countof(nazwaUzytkownika));

    while (1) {
        char szBuf[8];
        char nazwapokoju[20];
        char message[200];
        printf("Podaj komende: \n");
        scanf_s("%s", szBuf, (unsigned)_countof(szBuf));
        if (!(strcmp(szBuf, "newMsg"))) {
            send(SSocket, szBuf, 200, 0);
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

            printf("Podaj nazwe pokoju: \n");
            scanf_s("%s", nazwapokoju, (unsigned)_countof(nazwapokoju));
            send(SSocket, nazwapokoju, 200, 0);

            send(SSocket, nazwaUzytkownika, 200, 0);

            printf("Podaj wiadomosc : \n");

            //fgets(message, 199, stdin);
            scanf_s("%s", message, (unsigned)_countof(message));
            //strcpy_s(message, 200, "No siemankoooooooooooooooo     oo o o oo o o ");
            send(SSocket, message, 200, 0);
        }
        else if (!(strcmp(szBuf, "newRoom"))) {
            send(SSocket, szBuf, 200, 0);
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

            //strcpy_s(nazwapokoju, 20, "pokoj333.txt");
            printf("Podaj nazwe pokoju: \n");
            scanf_s("%s", nazwapokoju, (unsigned)_countof(nazwapokoju));
            send(SSocket, nazwapokoju, 200, 0);
        }
        else if (!(strcmp(szBuf, "onRoom")))
        {
            send(SSocket, szBuf, 200, 0);
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

            printf("Podaj nazwe pokoju: \n");
            scanf_s("%s", nazwapokoju, (unsigned)_countof(nazwapokoju));
            send(SSocket, nazwapokoju, 200, 0);

            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);
            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);
        }
        else if (!(strcmp(szBuf, "clRoom")))
        {
            send(SSocket, szBuf, 200, 0);
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

            printf("Podaj nazwe pokoju: \n");
            scanf_s("%s", nazwapokoju, (unsigned)_countof(nazwapokoju));
            send(SSocket, nazwapokoju, 200, 0);

        }
        else if (!(strcmp(szBuf, "refresh")))
        {
            send(SSocket, szBuf, 200, 0);
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

            printf("Podaj nazwe pokoju: \n");
            scanf_s("%s", nazwapokoju, (unsigned)_countof(nazwapokoju));
            send(SSocket, nazwapokoju, 200, 0);

            printf("Podaj miejsce w ktorym jestes: \n");
            scanf_s("%s", nazwapokoju, (unsigned)_countof(nazwapokoju));
            send(SSocket, nazwapokoju, 200, 0);

            while (1) {
                nBytes = recv(SSocket, cbBuf, 200, 0);
                cbBuf[nBytes] = '\x0';
                if(!(strcmp(cbBuf, ""))){
                    break;
                }
                else {
                    printf("%s", cbBuf);
                }
            }
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);
            cbBuf[nBytes] = '\x0';
            printf("New position:\t%s", cbBuf);

        }
        else if (!(strcmp(szBuf, "listPok")))
        {
            send(SSocket, szBuf, 200, 0);
            nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

            cbBuf[nBytes] = '\x0';
            printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

            while (1) {
                nBytes = recv(SSocket, cbBuf, 200, 0);
                cbBuf[nBytes] = '\x0';
                if (!(strcmp(cbBuf, ""))) {
                    break;
                }
                else {
                    printf("%s\n", cbBuf);
                }
            }
            printf("Koniec wysylania\n");
        }
        else if (!(strcmp(szBuf, "exit")))
        {
            send(SSocket, szBuf, 200, 0);
            printf("Koniec programu");
            break;
            
        }
        else {
            printf("Error - klient: podana komenda: %s", szBuf);
            send(SSocket, "exit", 200, 0);
            break;
        }
        memset(szBuf, '\0', sizeof(char) * 8);
        memset(nazwapokoju, '\0', sizeof(char) * 20);
        memset(message, '\0', sizeof(char) * 200);
    }
    /*
    char szBuf[8];
    strcpy_s(szBuf, 8, "newMsg");

    send(SSocket, szBuf, 100, 0);
    nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

    cbBuf[nBytes] = '\x0';

    printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

    char message[200];
    strcpy_s(message, 200, "No siemankoooooooooooooooo     oo o o oo o o ");
    send(SSocket, message, 200, 0); 
    */

    /*char szBuf[8];
    strcpy_s(szBuf, 8, "newRoom");

    send(SSocket, szBuf, 100, 0);
    nBytes = recv(SSocket, cbBuf, sizeof(cbBuf), 0);

    cbBuf[nBytes] = '\x0';

    printf("Data from SERVER [%s]:\t%s", hostname, cbBuf);

    char nazwapokoju[20];
    strcpy_s(nazwapokoju, 20, "pokoj333.txt");

    send(SSocket, nazwapokoju, 20, 0);
    */




    closesocket(SSocket);

    /* terminate use of the winsock */
    WSACleanup();

    return 0;
}
