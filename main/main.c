//
//  Internet Protocols project - Client server application
//  Created by Benta Andrei and Cozma Floriana
//  Year 2021
//

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
#include "client.h"
#include "myserver.h"
#include "myconstants.h"





int main(int argc, char *argv[])
{
  if(argc >= 2)
  {
      serverIPv4(argv[1]);
  }
  else
  {
    serverIPv4(WEB_URL);
  }
  atexit(cleanups);
  return 0;
}

