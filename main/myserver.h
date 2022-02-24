#ifndef MYSERVER_H
#define MYSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include "myconstants.h"


int recieveCommands(int , uint16_t , char* );
int serverIPv4(const char* );
void cleanups();
int *socketOpened;

#endif
