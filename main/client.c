#include"client.h"


void logMessage(const char *function, const char *msg) //log function, prints to file with current time, function name and message
{
    FILE *logFile = fopen("execution.log", "a");
    time_t T = time(NULL);
    struct tm currTime = *localtime(&T);
    fprintf(logFile,"[ %02d/%02d/%04d ] [ %02d:%2d:%2d ]              ", currTime.tm_mday,
                                                                currTime.tm_mon+1,
                                                                currTime.tm_year+1900,
                                                                currTime.tm_hour,
                                                                currTime.tm_min,
                                                                currTime.tm_sec);

    fprintf(logFile,"%s               %s\n", function,  msg );
    fclose(logFile);

}


const char * get_readable_address(const struct sockaddr_in6 *sa, char *s, size_t maxlen)
{
    
    return inet_ntop(AF_INET6, &(sa)->sin6_addr, s, maxlen); //converts from binary to humanly readable address
    
}
int clientIPv6(const char * website)
{
  logMessage( __FUNCTION__ ,"Starting client!");
  struct addrinfo *result, *rp; //pointers used for DNS lookup, rp used to navigate through result (linked list)
  struct addrinfo hints; //hints used for DNS lookup
  int addrinfoStatus;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6;    /* For ipv6*/
  hints.ai_flags = AI_V4MAPPED; //IPv4-mapped IPv6 addresses
  hints.ai_protocol = IPPROTO_TCP ;         /* TCP */

  char printed_address[INET6_ADDRSTRLEN]; //placeholder for the displayed address


  addrinfoStatus = getaddrinfo(website, NULL, &hints, &result);
  

  if (addrinfoStatus != 0) {
    logMessage( __FUNCTION__ ,gai_strerror(addrinfoStatus));//converts from error code to human readable message
    printf("getaddrinfo: %s\n", gai_strerror(addrinfoStatus));
    return EXIT_FAILURE;
  }

  int sfd;
  struct sockaddr_in6 mysocket;
  mysocket.sin6_port = htons(IPV6PORT);
  mysocket.sin6_family = AF_INET6;
  sfd = socket(mysocket.sin6_family, SOCK_STREAM, 0); //domain, type, protocol 



  if (sfd == -1){
        logMessage( __FUNCTION__ ,"Socket failed!");
        printf("Socket failed!\n");
        return EXIT_FAILURE;
      }

  for (rp = result; rp != NULL; rp = rp->ai_next)
  {
      printf("Found %s\n\n", get_readable_address((struct sockaddr_in6 *)rp->ai_addr, printed_address, INET6_ADDRSTRLEN));     
      logMessage(__FUNCTION__, get_readable_address((struct sockaddr_in6 *)rp->ai_addr, printed_address, INET6_ADDRSTRLEN));
      memcpy(&mysocket.sin6_addr, (&((struct sockaddr_in6 *)(rp->ai_addr))->sin6_addr), INET6_ADDRSTRLEN );    
  }

  freeaddrinfo(result); //frees the list returned by the lookup
  struct timespec tstart={0,0}, tend={0,0}; //for time measuring of the operation
  

  printf("Connecting to: %s\n", get_readable_address(&mysocket, printed_address, INET6_ADDRSTRLEN));
  printf("On port: %d \n", ntohs(mysocket.sin6_port));
  int connstatus = connect(sfd, (struct sockaddr *)&mysocket, sizeof(mysocket));//connect to the server
  if (connstatus != -1)
      {
        printf("Connected!\n");                 //success
        
        char recievedBytesBuffer[RECIEVED_BUFF_SIZE];
        char *getRequest = "GET / HTTP/1.0\r\n\r\n"; //declares the client request command
        // response contains header and body and the connection closes after the response 

        if(send(sfd,getRequest,strlen(getRequest),0) <0)
        {
            logMessage( __FUNCTION__ ,"Error sending GET");
            printf("\nError sending GET\n");
            return EXIT_FAILURE;
        }
        else{
            logMessage( __FUNCTION__ ,"Get sent successfully!");
            printf("\nGet sent successfully!\n");
        }

      

        int recieve = 1;
        ssize_t recievedBytes = 0;
        
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        recievedBytes = recv(sfd, recievedBytesBuffer, RECIEVED_BUFF_SIZE, 0);

        if(recievedBytes < 0)
        {
            logMessage( __FUNCTION__ ,"Failed to recieve bytes!");
            printf("\nFailed to recieve bytes!\n");
            return EXIT_FAILURE;
        }
        else
        {
            FILE *myFile = fopen(FILENAME, "w");
            if(myFile == NULL)
            {
              logMessage( __FUNCTION__ ,"Error creating/writing to file!");
              printf("Error creating/writing to file!\n");
              
            }
            while(recievedBytes > 0) //verifies if the connection is closed
            {
              recievedBytesBuffer[recievedBytes] = '\0';
              fputs(recievedBytesBuffer,myFile); //writes the received bytes into the file 
              //printf("%s", recievedBytesBuffer);
              
              recievedBytes = recv(sfd, recievedBytesBuffer, RECIEVED_BUFF_SIZE, 0); 
            }
            fclose(myFile);

        }

        clock_gettime(CLOCK_MONOTONIC, &tend); //display the time taken for operation
        printf("Operation took: %.5f seconds\n\n",
           ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

      }
  else
  {
        logMessage( __FUNCTION__ ,"Failed to connect!");
        printf("Failed!\n"); //failure
        return EXIT_FAILURE;
  }

  close(sfd); //closes connection from the client side
  logMessage( __FUNCTION__ ,"Finished IPV6 operation!");
  printf("Finished\n");
  return 0;

}
