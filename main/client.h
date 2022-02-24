#ifndef CLIENT_H
#define CLIENT_H

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


void logMessage(const char *, const char *);
int clientIPv6(const char *);
const char * get_readable_address(const struct sockaddr_in6 *, char *, size_t);

#endif
