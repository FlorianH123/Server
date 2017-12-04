#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#define QUEUELENGTH 5
#define STANDARD_PORT 32000

/*--- Protofunktionen --- */
void readFileAndSendData(SOCKET);

/*--- Ende Protofunktionen ---*/

/*--- Konstanten ---*/
const char* fileNotFoundException = "Die Datei wurde nicht gefunden!\n";
/*--- Ende Konstanten ---*/

void readFileAndSendData(SOCKET workerSocketDescriptor) {
    const int buffer_len = 250;
    char* buffer = NULL;

    char* arg;
    char delimiter[] = "-, ";
    int nBytes = 0, n = 0, c = 0;
    FILE *fp;

    //memset(buffer, '\0', sizeof(buffer));
    recv(workerSocketDescriptor, buffer, buffer_len, 0);

    nBytes = atoi(strtok(buffer, delimiter));
    arg = strtok(NULL, delimiter);

    while (arg != NULL) {
        fp = fopen(arg, "r");
        char charArray[nBytes + 1];

        if (fp == NULL) {
            send(workerSocketDescriptor, fileNotFoundException, strlen(fileNotFoundException), 0);
            fprintf(stderr, "Datei %s nicht gefunden\n", arg);
        } else {
            while (n < nBytes && (c = fgetc(fp)) != EOF) {
                charArray[n] = (char) c;
                n++;
            }

            charArray[n] = '\0';
            send(workerSocketDescriptor, (const char *) charArray, nBytes + 1, 0);

            fclose(fp);
            n = 0;
        }

        arg = strtok(NULL, delimiter);
    }

    closesocket(workerSocketDescriptor);
}

int main() {
    int port = STANDARD_PORT;

    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(32000);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKET workerSocketDescriptor;
    SOCKET serverSocketDescriptor;

    /* Socket erzeugen */
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocketDescriptor < 0) {
        fprintf(stderr, "socket creation failed\n");
        exit(3);
    }

    /* Lokale Adresse mit Socket binden */
    if (bind(serverSocketDescriptor, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
        fprintf(stderr, "bind failed\n");
        exit(4);
    }

    /* Groesse der Anfragewarteschlange bestimmen */
    if (listen(serverSocketDescriptor, QUEUELENGTH) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(5);
    }

    while(1) {
        if ((workerSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientaddr,
                                             (int *) sizeof clientaddr)) < 0) {
            fprintf(stderr, "accept failed\n");
            exit(6);
        }

        readFileAndSendData(workerSocketDescriptor);
    }
}

