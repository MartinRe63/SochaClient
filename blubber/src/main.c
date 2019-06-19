#include <stdlib.h> 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "serverIO.h"
#include "logic.h"
#include "masc.h"

//#define TEST

int main(int argc, char**argv) {
#ifndef TEST
    char * host = "localhost";
    char * port = "13050";
    char * reservation = 0;
    int joinReserved = 0;
    // Parse Arguments
    for (int i = 1; i < argc; i++) {
        if (!strcmp("-h", argv[i]) || !strcmp("--host", argv[i])) {
            i++;
            assert(i < argc);
            //free(host);
            host = 0;
            host = malloc(sizeof (char) * strlen(argv[i]) + 1);
            strcpy(host, argv[i]);
        } else if (!strcmp("-p", argv[i]) || !strcmp("--port", argv[i])) {
            i++;
            assert(i < argc);
            //free(port);
            port = 0;
            port = malloc(sizeof (char) * strlen(argv[i]) + 1);
            strcpy(port, argv[i]);
        } else if (!strcmp("-r", argv[i]) || !strcmp("--reservation", argv[i])) {
            joinReserved = 1;
            i++;
            assert(i < argc);
            /*if (reservation != 0)
                free(reservation);*/
            reservation = 0;
            reservation = malloc(sizeof (char) * strlen(argv[i]) + 1);
            strcpy(reservation, argv[i]);
        }
    }
    initMasks();
    if (!sIO_connect(port, host)) {
        exit(-1);
    }
    if (joinReserved) {
        sIO_joinReservation(reservation);
    } else {
        sIO_join();
    }
    while (!logic_isInitialized() && sIO_check()) {
    }
    printf("Los Gehts!\n");
    while (sIO_check()) {
        logic_update();
    }
    printf("Schon vorbei :(\n");
    sIO_close();
#else
    argc = argc + 0;
    argv = argv + 0;
    initMasks();
    Board board = board_parseString(".RRRRRRRR.B........BB........BB........BB..O.....BB........BB...O....BB........BB........B.RRRRRRRR.");
    unsigned char gameEnd = 0;
    for(int i = 0; i < 100; i++){
        printf("Rating: %f\n", board_rateState(board, 0, 0, &gameEnd));
        printf("%d\n", (int)((((float)(rand() / 2)) / (RAND_MAX / 2 + 1)) * 16));
    }
#endif
    return 0;
}
