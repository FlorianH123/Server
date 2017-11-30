#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#define QUEUELENGTH 5
#define STANDARD_PORT 5193

/*--- Protofunktionen --- */
void readFile(SOCKET);

/*--- Ende Protofunktionen ---*/

/*--- Konstanten ---*/
const char* usage = "Geben Sie max. 5 Dateinamen (mit -) und die Anzahl der ersten n "
        "Bytes an, die Sie gesndet haben wollen";

const char* fileNotFoundException = "Die Datei wurde nicht gefunden!";
/*--- Ende Konstanten ---*/

void readFile(SOCKET workerSocketDescriptor) {
    char* buffer = NULL;
    int buffer_len = 250, nBytes = 0, n = 0, c = 0;
    char* arg;
    FILE *fp;

    send(workerSocketDescriptor, usage, strlen(usage), 0);
    recv(workerSocketDescriptor, buffer, buffer_len, 0);

    nBytes = atoi(strtok(buffer, "-"));
    char charArray[nBytes + 1];
    arg = strtok(NULL, "-");

    while (arg != NULL) {
        fp = fopen("arg", "r");

        if (fp == NULL) {
            send(workerSocketDescriptor, fileNotFoundException, strlen(fileNotFoundException), 0);
        } else {
            while (n <= nBytes) {
                c = fgetc(fp);
                *charArray = (char) c;
                *charArray++;
                n++;
            }
            *charArray = '\0';
            send(workerSocketDescriptor, (const char *) charArray, nBytes, 0);
        }

        arg = strtok(NULL, "-");
    }
}

int main() {
    int port = STANDARD_PORT; /* Protokoll-Portnummer */

    struct sockaddr_in serveraddr;  /* Struktur fuer die Serveradresse */
    struct sockaddr_in clientaddr;  /* Struktur fuer die Clientadresse */

    serveraddr.sin_family = AF_INET;             /* Familie auf Internet setzen */
    serveraddr.sin_port = htons((u_short) port);  /* host-byte-order -> network byte-order */
    serveraddr.sin_addr.s_addr = INADDR_ANY;     /* Lokale IP-Adressen setzen */

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

        readFile(workerSocketDescriptor);
    }
}
