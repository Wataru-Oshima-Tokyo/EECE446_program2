#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <stdlib.h>
// #define HOST "www.csuchico.edu"
// #define HOST "ecc-linux.csuchico.edu"
#define HOST "localhost"
#define _PORT "2020"
#define BYTESIZE 1000
#define rep(i,a,b) for(int i=a; i<b;i++)
int lookup_and_connect( const char *host, const char *service );
void join(int socketFd, int peer_id);
void search(int socketFd, char *filename);
void publish(int socketFd, int count, char *files[]);




void join(int socketFd, int peer_id)
{
    char join_request[5];

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
        // printf("successfully joined!!\n");
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
    } 
    // int temp = sizeof(publsish_request);    
    // printf("%d\n", lenOfarray);
    bytes_sent = send(socketFd, publsish_request, lenOfarray, 0);
    if (bytes_sent == -1)
    {
        perror("[<Error> Client] onSend:\n");
        exit(1);
    }else{
        // printf("successfully published");
    }


}

void search(int socketFd, char *filename){
    char search_request[1200];
    char buf[10];
    // char s[1200];
    search_request[0] = 0x02;
    int len = strlen(filename);
    rep(i,1,len+1){
        search_request[i] = filename[i-1];
    } 
    search_request[len+1] = 0x00;
    int bytes_sent = send(socketFd, search_request, len+2, 0);
    if (bytes_sent == -1)
    {
        perror("[<Error> Client] onSend:\n");
        exit(1);
    }else{
        // printf("%02X\n", bytes_sent);
        // printf("successfully searched\n");
    }
    int _len =0;
    _len = recv(socketFd, buf, 10, 0 );
    if(_len <0){exit(1);}
    // unsigned int result =0x00;
    char peer[100];
    char port[100];
    long peer_n =0;
    long port_n=0;
    // result[0] ='x';
    for (int i = 0; i < 10; i++)
    {
        char str[6];
        unsigned int temp = buf[i] & 0xFF;         
        if(i<4){
            // printf("%d", temp); 
            snprintf(str, sizeof(str), "%02x", temp);
            strcat(peer, str);
            
        }   
        else if (i==4){
             peer_n = strtol(peer, NULL, 16);
             if(peer_n !=0){
                 printf("File found at\n Peer %ld\n", peer_n);
                 peer_n =0;
             }else {
                 printf("File not indexed by registry\n");
                //  free(peer);
                //  free(port);
                 break;
             }
            //  printf("%ld", n);
             printf(" %d:", temp); 
        }
        else if (i<8){
            
            if(i==7) printf("%d:", temp);
            else printf("%d.", temp); 
        }
        else{
            snprintf(str, sizeof(str), "%02x", temp);
            strcat(port, str);
            if(i==9){
                port_n = strtol(port, NULL, 16);
                printf("%ld\n", port_n);

                // free(peer);
                // free(port);
                break;
            }
        }

    }  
    memset(port, 0, 100);
    memset(peer, 0, 100);     
}






int main(int argc, char *argv[])
{
    // Address
    int s;
    const char *HOST_ADDR = HOST;
    const char *PORT = _PORT;
    int peer_id = 0;
    int CHUNK_SIZE = BYTESIZE;
    if (argc ==4){
        HOST_ADDR = argv[1];
        PORT = argv[2];
        peer_id =atoi(argv[3]);
        CHUNK_SIZE = 1000;
    }else {
        printf("please put server name, port nummber and peer id\n");
        exit(1);
    }
    //setting the chunksize
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
        // printf("successfully connected to the host!\n");
    }
    
    //countig the number of files inside sharedFiels directory
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

    //checking the name of files inside sharedFiels directory
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



    printf("Enter a command: ");
    char input[100];
	// int action= 0x00;
    scanf("%s",input);
    while(strcmp(input,"EXIT")){
        if(strcmp(input,"JOIN")==0){
            join(s, peer_id);
        }else if (strcmp(input,"PUBLISH")==0){
            publish(s, file_count, fileNames);
            // printf("send a PUBLISH request to the registry.\n");
        }else if (strcmp(input,"SEARCH")==0){
            printf("Enter a file name: ");
            char _search[100];
            scanf("%s",_search);
            search(s, _search);
            // printf(" (a) read a file name from the terminal,\n (b) send a SEARCH request to the registry,\n (c) and print the peer info from the SEARCH response or a message if the file was not found.\n");
        }else if (strcmp(input,"EXIT")==0){
            printf("close the peer application");
            break;
        }else {
            printf("Please input a valid value.\n");
        }
        printf("Enter a command: ");
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

