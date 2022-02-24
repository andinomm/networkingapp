
#include"myserver.h"
#include"client.h"


int recieveCommands(int outerConnection, uint16_t recieved, char* command)
{
  
  //ssize_t = signed size t => [-1;max_size]
    ssize_t recievedByte = recv(outerConnection, &recieved, sizeof(recieved), 0);
    if(recievedByte < 1){
        logMessage( __FUNCTION__ ,"Connection terminated!");
        printf("/////////////////////////////////////////////////////////////\n");
        printf("/////////////[Connection terminated by client!]//////////////\n");
        printf("/////////////////////////////////////////////////////////////\n");
        strcpy(command,"EOC");
        return 0;
    }
    recieved = ntohs(recieved);
    //command is 3 chars long => 1 char = 1 byte => 3 chars = 3 bytes => we get characters
    //using byte masks, uint16_t = 2 bytes => 2*uint_16t = 4 bytes, we need only 3 => 2*recv
    char c2 = recieved&0xFF;
    char c = (recieved>>8)&0xFF;
    command[0] = c;
    command[1] = c2;
    recievedByte = recv(outerConnection, &recieved, sizeof(recieved), 0);
    recieved = ntohs(recieved);
    c = (recieved>>8)&0xFF;
    command[2]= c;
    command[3]= '\0';

  return 1;
  
}

int serverIPv4(const char* webPage)
{
    logMessage( __FUNCTION__ ,"Starting IPv4 server!");
    int local, outerConnection;
	struct sockaddr_in server, client;
	int clientLength;

	local = socket(AF_INET, SOCK_STREAM, 0);
    socketOpened = &local;
	if (local < 0) {
        logMessage( __FUNCTION__ ,"Error socket!");
		printf("Error socket\n");
		return 1;
	}

    //setting server parameters eg: port, type of connection and to accept from any ip
	memset(&server, 0, sizeof(server));
	server.sin_port = htons(22522);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

    int forceBind = 1; //this method is used to force binding if the "error binding" error appears
    if(setsockopt(local,SOL_SOCKET,SO_REUSEADDR,&forceBind,sizeof (forceBind))==-1)
    {
        logMessage( __FUNCTION__ ,"Error forcing bind!");
        printf("Error forcing bind!");
        return 1;
    }


	if (bind(local, (struct sockaddr*) &server, sizeof(server)) < 0) {
        logMessage( __FUNCTION__ ,"Bind error!");
		printf("Bind error\n");
		return 1;
	}

  char recievedBuffer[16];

    listen(local, 5);  //listen for connections, 5 in queue
	clientLength = sizeof(client);
	memset(&client, 0, clientLength);

  char command[4];
  int exitStats = 1;
	
	while (1) {  //for connection
		uint16_t n;
        logMessage( __FUNCTION__ ,"Listening for connection...");
		printf("Listening for connection...\n");
		outerConnection = accept(local, (struct sockaddr*) &client, &clientLength);
		if (outerConnection < 0) {
            logMessage( __FUNCTION__ ,"Error on connection accept!");
			printf("Error on accept\n");
			continue;
		}
        logMessage( __FUNCTION__ ,inet_ntoa(client.sin_addr));
		printf("Connected from %s\n", inet_ntoa(client.sin_addr));

    
	do
    {	
    exitStats = recieveCommands(outerConnection, n, command); //do stuff until client terminates connection
    

    if(exitStats!=0) //if commands are recieved do stuff
    {
    logMessage(__FUNCTION__,command);
    printf("Command is: %s\n", command);
    if(strcmp(command, COMMAND) == 0) //if command is the correct one
    {   
        printf("\nCommand accepted!\n");
        ssize_t retriesClient = 0;
        int clientStatus = 0;
        do
        {
        clientStatus = clientIPv6(webPage);//http client gets called
        retriesClient++;
        }
        while(clientStatus && retriesClient < RETRIES);//retries stuff
        
        if(retriesClient >= RETRIES)
        {
             if(send(outerConnection,ERROR_CONNECTING_PAGE,strlen(ERROR_CONNECTING_PAGE),0) <0)
            {
                logMessage( __FUNCTION__ ,"Error sending response to client!");
                printf("Error sending response to client!\n");
            }

            continue;
        }

        FILE *file = fopen(FILENAME, "r");
        size_t pos = 0;
        int errStatus = 0;
        if(file != NULL)
        {
        char bufferRead[RECIEVED_BUFF_SIZE+1];
        size_t readLength = fread(bufferRead, sizeof *bufferRead, RECIEVED_BUFF_SIZE, file);
        while(readLength == RECIEVED_BUFF_SIZE)
        {
            bufferRead[readLength] = '\0';
            //printf("%s",bufferRead);
            if(send(outerConnection,bufferRead,readLength,0) <0)
            {
                errStatus = 1;
                break;
            }
            readLength = fread(bufferRead, sizeof *bufferRead, RECIEVED_BUFF_SIZE, file);
        }

        bufferRead[readLength] = '\0';
        //printf("%s",bufferRead);
        if(send(outerConnection,bufferRead,readLength,0) <0)
        {
         errStatus = 1;
        }

        fclose(file);

        if(errStatus)
        {
            logMessage( __FUNCTION__ ,"Error sending response to client!");
            printf("Error sending response to client!\n");
        }
        else
        {
            logMessage( __FUNCTION__ ,"Response sent successfully!");
            printf("Response sent successfully!\n");
        }

        
        }
        else
        {
            logMessage( __FUNCTION__ ,"Error opening temp html file!");
            printf("Error opening temp html file\n");
        }

    }
    else
    {
            logMessage( __FUNCTION__ ,"Command not accepted!!");
            printf("\nCommand not accepted!\n");
            if(send(outerConnection,REJECTED_COMMAND,strlen(REJECTED_COMMAND),0) <0)
            {
                logMessage( __FUNCTION__ ,"Error sending response to client!");
                printf("Error sending response to client!\n");
            }
            else{
                printf("Response sent successfully!\n");
            }
    
    }
    

    printf("\n///////////////////////////////////////////////////////////\n");
    printf("/////////////////////[Exchange done]/////////////////////////\n");
    printf("/////////////////////////////////////////////////////////////\n");
    }
    }
    while(exitStats);

        close(outerConnection);
        logMessage( __FUNCTION__ ,"Server terminated!");

	}

}

void cleanups()
{

    close(*socketOpened);
    logMessage( __FUNCTION__ ,"Server terminated forcefully!");
}

