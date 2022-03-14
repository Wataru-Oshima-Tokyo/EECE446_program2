#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

// #define HOST "www.csuchico.edu"
// #define HOST "ecc-linux.csuchico.edu"
#define HOST "localhost"
#define _PORT "2010"
#define BYTESIZE 1000
#define rep(i,a,b) for(int i=a; i<b;i++)
int lookup_and_connect( const char *host, const char *service );
void join(int socketFd, int peer_id);
void search(int socketFd, char *filename);
void publish(int socketFd, int count, char *files[]);
void convertToHex(char* input, char* output);
// int convertToHex(char* input);

void join(int socketFd, int peer_id)
{
    char join_request[5];
    char join_response[5];
    int bytes_recieved = 0;
    int bytes_sent = 0;
    // Action = 0 in hexodecimal 
    join_request[0] = 0x00;
    // Peer ID must be in network byte order, this should be 4 bytes total
    join_request[1] = (peer_id >> 24) & 0xFF;
    join_request[2] = (peer_id >> 16) & 0xFF;
    join_request[3] = (peer_id >> 8) & 0xFF;
    join_request[4] = peer_id & 0xFF;
    
    // rep(i, 0, 5) printf("%d\n", join_request[i]);
    // send join request
    bytes_sent = send(socketFd, join_request, 5, 0);
    if (bytes_sent == -1)
    {
        perror("[<Error> Client] onSend:\n");
        exit(1);
    }else{
        printf("successfully joined!!\n");
    }
}

void publish(int socketFd, int count, char *files[]){
    char publsish_request[1200];
    int bytes_sent = 0;
	// action ==1 for publsih 
	publsish_request[0] = 1 & 0xFF;
	publsish_request[1] = (count >> 24) & 0xFF;
    publsish_request[2] = (count >> 16) & 0xFF;
    publsish_request[3] = (count >> 8) & 0xFF;
    publsish_request[4] = count & 0xFF;
    int start =5;
    int lenOfarray = 5;
    rep(i,0,count){
        int len = strlen(files[i]);
        rep(j,0,len){
            publsish_request[start] = files[i][j];
            start++;
            lenOfarray++;
        } 
        publsish_request[start] =0x00;
        start++;
        lenOfarray++;
        // char hex_str[(len*2)+1];
        // printf("%s\n", files[i]);
        // convertToHex(files[i], hex_str);
		// int char_count = sizeof(files[i]);
        // printf("hex_str: %s\n", hex_str);
		// printf("%d\n", char_count);
        // bytes_sent = send(socketFd, publsish_request, 1200, 0);

    } 
    // int temp = sizeof(publsish_request);    
    printf("%d\n", lenOfarray);
    bytes_sent = send(socketFd, publsish_request, lenOfarray, 0);
    if (bytes_sent == -1)
    {
        perror("[<Error> Client] onSend:\n");
        exit(1);
    }else{
        printf("successfully published");
    }
    //         
    // if (bytes_sent == -1)
    // {
    //     perror("[<Error> Client] onSend:\n");
    //     exit(1);
    // }else{
    //     printf("%02X is published!!\n", publsish_request);
    // }

}

void search(int socketFd, char *filename){

}






int main(int argc, char *argv[])
{
    // Address
    int s;
    // print out the argum
    // const char *HOST_ADDR = argv[1];
    const char *HOST_ADDR = HOST;
    // const char *PORT = argv[2];
    const char *PORT = _PORT;
	printf("%s\n", PORT);
    // define HOST_ADDR and PORT as arguments
    int CHUNK_SIZE = BYTESIZE;
    char *request = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
    // A peer must have a unique identifier, coming in from the arguments
    // int peer_id = atoi(argv[3]);
    int peer_id = atoi(argv[1]);
    // passing args correct #
    // if (argc != 4)
    if (argc != 2)
    {
        printf("3 Args plz\n");
        exit(1);
    }
    // else if (argc == 4)
    else if (argc == 2)
    {
        // Read in the argument
        CHUNK_SIZE = 1000;
    }
    char response[CHUNK_SIZE];
    if (CHUNK_SIZE <= 0 || CHUNK_SIZE > 1000)
    {
        printf("Error: Please enter CHUNK_SIZE > 0 and <= 1000\n");
        exit(1);
    }
    if ((s = lookup_and_connect(HOST_ADDR, PORT)) < 0)
    {
        fprintf(stderr, "Error connecting %s\n", gai_strerror(s));
        exit(0);
    }else{
        printf("successfully connected to the host!\n");
    }
    
    	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir("./SharedFiles/"); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_type == DT_REG) { /* If the entry is a regular file */
			// printf("%s\n",entry->d_name);
			file_count++;
		}
	}
	char *fileNames[file_count];
	closedir(dirp);
	int count = 0;
	dirp = opendir("./SharedFiles/"); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
		if (entry->d_type == DT_REG) { /* If the entry is a regular file */
			fileNames[count] = entry->d_name;
			count++;
		}
	}
	closedir(dirp);



    printf("Please input a value: ");
    char input[100];
	int action= 0x00;
    scanf("%s",input);
    while(strcmp(input,"EXIT")){
        if(strcmp(input,"JOIN")==0){
            join(s, peer_id);
        }else if (strcmp(input,"PUBLISH")==0){

            publish(s, file_count, fileNames);
            printf("send a PUBLISH request to the registry.\n");
        }else if (strcmp(input,"SEARCH")==0){
            printf(" (a) read a file name from the terminal,\n (b) send a SEARCH request to the registry,\n (c) and print the peer info from the SEARCH response or a message if the file was not found.\n");
        }else if (strcmp(input,"EXIT")==0){
            printf("close the peer application");
            break;
        }else {
            printf("Please input a valid value.\n");
        }
        printf("Please input a value: ");
        scanf("%s",input);
    }

	close(s);





    return 0;
}





int lookup_and_connect( const char *host, const char *service ) {
	struct addrinfo hints;
	struct addrinfo *rp, *result;
	int s;

	/* Translate host name into peer's IP address */
	memset( &hints, 0, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	if ( ( s = getaddrinfo( host, service, &hints, &result ) ) != 0 ) {
		fprintf( stderr, "stream-talk-client: getaddrinfo: %s\n", gai_strerror( s ) );
		return -1;
	}

	/* Iterate through the address list and try to connect */
	for ( rp = result; rp != NULL; rp = rp->ai_next ) {
		if ( ( s = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol ) ) == -1 ) {
			continue;
		}

		if ( connect( s, rp->ai_addr, rp->ai_addrlen ) != -1 ) {
			break;
		}

		close( s );
	}
	if ( rp == NULL ) {
		perror( "stream-talk-client: connect" );
		return -1;
	}
	freeaddrinfo( result );

	return s;
}


void convertToHex(char* input, char* output){
    int loop;
    int i; 
    
    i=0;
    loop=0;
    
    while(input[loop] != '\0')
    {
        sprintf((char*)(output+i),"%02X:", input[loop]);
        loop+=1;
        i+=2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}


// int convertToHex(char* input, char* output){
//     int loop;
//     int i; 
    
//     i=0;
//     loop=0;
    
//     while(input[loop] != '\0')
//     {
//         sprintf((char*)(output+i),"%02X:", input[loop]);
//         loop+=1;
//         i+=2;
//     }
//     //insert NULL at the end of the output string
//     output[i++] = '\0';
// }
