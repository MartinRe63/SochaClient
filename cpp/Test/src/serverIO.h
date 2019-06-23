#ifndef __SERVER_IO_H__
#define __SERVER_IO_H__

#include "NodeManager.h"

//Connect to the Server.
int sIO_connect(char * Port, char * Host);
int sIO_sendMove(LastMove m);
int sIO_join();
int sIO_joinReservation(char * reservation);
int sIO_check();
int sIO_close();

#endif