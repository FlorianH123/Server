#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define QUEUELENGTH 5
#define STANDARD_PORT 32000

/*--- Protofunktionen --- */
void readFileAndSendData(int);

/*--- Ende Protofunktionen ---*/

/*--- Konstanten ---*/
const char* fileNotFoundException = "Die Datei %s wurde nicht gefunden!\n";
/*--- Ende Konstanten ---*/

void readFileAndSendData(int workerSocketDescriptor) {
    const int buffer_len = 255;
    char buffer[buffer_len];

    char* arg;
    char delimiter[] = "-, ";
    int nBytes = 0, n = 0, c = 0;
    FILE *fp;

    recv(workerSocketDescriptor, buffer, buffer_len, 0);
    nBytes = atoi(strtok(buffer, delimiter));
    arg = strtok(NULL, delimiter);

    while (arg != NULL) {
        fp = fopen(arg, "r");
        char charArray[nBytes + 1];

        if (fp == NULL) {
            send(workerSocketDescriptor, strcat(strcat("Die Datei ", arg), " wurde nicht gefunden!"), strlen(fileNotFoundException), 0);
        } else {
            while (n < nBytes && (c = fgetc(fp)) != EOF) {
                charArray[n] = (char) c;
                n++;
            }

            charArray[n] = '\0';
            send(workerSocketDescriptor, charArray, nBytes + 1, 0);

            fclose(fp);
            n = 0;
        }

        arg = strtok(NULL, delimiter);
    }

    close(workerSocketDescriptor);
}

int main(int argc, char *argv[]) {
    int port = STANDARD_PORT;

    socklen_t client_length;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;

    int workerSocketDescriptor;
    int serverSocketDescriptor;

    /* Socket erzeugen */
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocketDescriptor < 0) {
        fprintf(stderr, "socket creation failed\n");
        exit(3);
    }

    bzero((char *) &serveraddr, sizeof(serveraddr));

    port = atoi(argv[1]);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(port);

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

    client_length = sizeof(clientaddr);

    while((workerSocketDescriptor = accept(serverSocketDescriptor,
                                           (struct sockaddr*) &clientaddr, &client_length)) > 0) {
        readFileAndSendData(workerSocketDescriptor);
    }


    if (workerSocketDescriptor < 0) {
        fprintf(stderr, "Fehler beim Accept");
        exit(6);
    }
}
