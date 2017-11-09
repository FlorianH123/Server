#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>
#include <stdint.h>

#define QUEUELENGTH 5
#define STANDARD_PROTO_PORT 5193

int main() {
    struct protoent *ptrp ;      /* Zeiger auf einen Eintrag in Protokolltabelle */
    struct sockaddr_in sad;      /* Struktur fuer die Serveradresse */
    struct sockaddr_in cad;      /* Struktur fuer die Clientadresse */
    SOCKET workerSocketDescriptor;
    SOCKET serverSocketDescriptor;

    int port = STANDARD_PROTO_PORT; /* Protokoll-Portnummer */
    socklen_t alen;              /* Laenge der Adresse */
    char buf[1000];              /* Puffer fuer die vom Server gesendeten Daten */

    memset((char *)&sad, 0, sizeof(sad)); /* sockaddr-Struktur leeren */

    sad.sin_family = AF_INET;             /* Familie auf Internet setzen */
    sad.sin_addr.s_addr = INADDR_ANY;     /* Lokale IP-Adressen setzen */
    sad.sin_port = htons((uint16_t)port);  /* host-byte-order -> network byte-order */


/* Name.von TCP-Transportprotokoll in Nummer umwandeln */

    if ( (ptrp = getprotobyname("tcp")) == NULL)
    {
        fprintf(stderr, "cannot map \"tcp\" to protocol number");
        exit(2);
    }

/* Socket erzeugen */
    serverSocketDescriptor = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (serverSocketDescriptor < 0)
    {
        fprintf(stderr, "socket creation failed\n");
        exit(3);
    }

/* Lokale Adresse mit Socket binden */
    if (bind(serverSocketDescriptor, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        fprintf(stderr, "bind failed\n");
        exit(4);
    }

/* Groesse der Anfragewarteschlange bestimmen */
    if (listen(serverSocketDescriptor, QUEUELENGTH) < 0)
    {
        fprintf(stderr, "listen failed\n");
        exit(5);
    }

    while(1) {
        alen = sizeof(cad);
        if ( (workerSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&cad, &alen)) < 0)
        {
            fprintf(stderr, "accept failed\n");
            exit(6);
        }

        send(workerSocketDescriptor, buf, strlen(buf), 0);
        closesocket(workerSocketDescriptor);
    }
}