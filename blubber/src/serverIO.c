#include <stdlib.h> 
#include <stdlib.h>
#include <string.h>

#ifdef _WIN
#pragma comment(lib,"Ws2_32.lib")
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#else
#include <netdb.h>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#endif

#include "xmlParser.h"
//#include "types.h"
#include "logic.h"

#define AF_ADDR AF_INET

#ifdef _WIN
SOCKET g_sockedfd;
#else
unsigned int g_sockedfd;
#endif

char g_room[128];
//Connect to the Server.

int sIO_connect(char * Port, char * Host) {
    //Socket erstellen

#ifdef _WIN
	int iResult;
	WSADATA wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}
#endif
    g_sockedfd = socket(AF_ADDR, SOCK_STREAM, IPPROTO_TCP );
    if (g_sockedfd < 0) {
		unsigned err = WSAGetLastError();
        printf( "Socket konnte nicht erstellt werden %d\n", err );
        return 0;
    }
    //Socket Verbinden
    /*struct sockaddr_in server;
    memset(&server, 0, sizeof (server));
    server.sin_family = AF_ADDR;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(Port);*/
    
    //struct addrinfo* hints;
    printf("Test1\n");
    struct addrinfo* res = 0;
    struct addrinfo* ptr = 0;
    if(getaddrinfo(Host, Port, NULL, &res)){
        printf("Addr Info konnte nicht ermittelt werden!\n");
        return 0;
    }
    printf("Test2\n");

    int connectResult = -1;
    unsigned err;
    for(ptr=res; ptr != NULL && connectResult == -1; ptr=ptr->ai_next)
    {
        connectResult = connect(g_sockedfd, res->ai_addr , res->ai_addrlen);
        if (connectResult < 0)
        {
    		err = WSAGetLastError();
        }
    }
    if ( connectResult < 0 )
    {
		printf("Verbindung konnte nicht hergestellt werden %d\n", err);
        return 0;
    }
    else
    {
        printf("Verbindung wurde hergestellt!\n");
        return 1;
    }
}

int sIO_join() {
    if (send(g_sockedfd, "<protocol><join gameType=\"swc_2019_piranhas\" />", 47, 0) < 0) {
        printf("Fehler beim senden!!!");
        return 0;
    }
    parseMassage(g_sockedfd);    
    return 1;
}
int sIO_joinReservation(char * reservation) {
    char buffer[1024];
    sprintf(buffer, "<protocol><joinPrepared reservationCode=\"%s\" />", reservation);
    if (send(g_sockedfd, buffer, strlen(buffer), 0) < 0) {
        printf("Fehler beim senden!!!");
        return 0;
    }
    parseMassage(g_sockedfd);    
    return 1;
}
//return 0 if the connection has been closed.
int sIO_check() {
    fd_set rfds;
    struct timeval tv;
    int retval;
    
    FD_ZERO(&rfds);
    FD_SET(g_sockedfd, &rfds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    retval = select(g_sockedfd + 1, &rfds, NULL, NULL, &tv);
    if(retval == -1)
        perror("select()");
    else if (retval){
        return parseMassage(g_sockedfd);
    }
    return 1;
}

int sIO_sendMove(LastMove m) {
    char buffer[1024];
    parseTurn(m, buffer);
    if (send(g_sockedfd, buffer, strlen(buffer), 0) < 0) {
        printf("Fehler beim senden");
        return 0;
    }
    return 1;
}

int sIO_close() {
    close(g_sockedfd);
    return 1;
}
