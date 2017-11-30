#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>

#define QUEUELENGTH 5
#define STANDARD_PORT 5193

/*--- Protofunktionen --- */
void readFileAndSendData(SOCKET);

/*--- Ende Protofunktionen ---*/

/*--- Konstanten ---*/
const char* usage = "Geben Sie die Anzahl der ersten n Bytes (-<n Bytes>) und max. 5 Dateinamen (mit -) an,"
        " die Sie erhalten wollen\n";

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
    send(workerSocketDescriptor, usage, strlen(usage), 0);
    recv(workerSocketDescriptor, buffer, buffer_len, 0);

    nBytes = atoi(strtok(buffer, delimiter));


    arg = strtok(NULL, "-");

    while (arg != NULL) {
        fp = fopen(arg, "r");
        char charArray[nBytes + 1];
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
            send(workerSocketDescriptor, (const char *) charArray, nBytes + 1, 0);
            fclose(fp);
            n = 0;

        }

        arg = strtok(NULL, delimiter);
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

        readFileAndSendData(workerSocketDescriptor);
    }
}
