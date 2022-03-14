
// This assignment is done by using the code from lab 2.
//Authors
//Oshima, Wataru
//Chu, Chen
//
//EECE446
//Spring 2022

/* This code is an updated version of the sample code from "Computer Networks: A Systems
 * Approach," 5th Edition by Larry L. Peterson and Bruce S. Davis. Some code comes from
 * man pages, mostly getaddrinfo(3). */
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
#define HOST "ecc-linux.csuchico.edu"
#define PORT "80"
#define BYTESIZE 512
#define rep(i,a,b) for(int i=a; i<b;i++)
/*
 * Lookup a host IP address and connect to it using service. Arguments match the first two
 * arguments to getaddrinfo(3).
 *
 * Returns a connected socket descriptor or -1 on error. Caller is responsible for closing
 * the returned socket.
 */
int lookup_and_connect( const char *host, const char *service );
char convertToHex(char* input, char* output);



int main(int argc, char *argv[]) {
	int s;
	/* Lookup IP and connect to server */
	if ( ( s = lookup_and_connect(HOST, PORT) ) < 0 ) {
		exit( 1 );
	}else{
		printf("successfully connected\n");
	}
	// printf("connection ok\n");

	/* Modify the program so it
	 *
	 * 1) connects to www.ecst.csuchico.edu on port 80 (mostly done above)
	 * 2) sends "GET /~kkredo/file.html HTTP/1.0\r\n\r\n" to the server
	 * 3) receives all the data sent by the server (HINT: "orderly shutdown" in recv(2))
	 * 4) prints the total number of bytes received
	 *
	 * */

	// char *msg = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
	// if(argc==2){
	// 	sscanf(argv[1], "%ld", &bytes_per_chunk); //https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int/42860313
	// 	if (bytes_per_chunk>521 || bytes_per_chunk <10){
	// 		printf("please put a number more than 10 and less than 521\n");
	// 		exit(1);
	// 	}
	// 	// bytes_per_chunk = (int)argv[1];

	// }
    // int len = strlen(msg);	 
	// if(send(s, msg, len, 0) < 0)
	// 	printf("error on send");
	// int total_recv = recv_timeout(s,3);
	// printf("Number of <h1> tags: %d\n", num);
	// printf( "Number of bytes: %d\n", total_recv);

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
            printf("send a JOIN request to the registry\n");
        }else if (strcmp(input,"PUBLISH")==0){
			action 	= 0x01;
			rep(i,0, file_count){
				int len = strlen(fileNames[i]);
    			char hex_str[(len*2)+1];
				convertToHex(fileNames[i], hex_str);
				printf("ascii_str: %s\n", fileNames[i]);
    			printf("hex_str: %s\n", hex_str);
			} 
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

char convertToHex(char* input, char* output){
    int loop;
    int i; 
    
    i=0;
    loop=0;
    
    while(input[loop] != '\0')
    {
        sprintf((char*)(output+i),"%02X", input[loop]);
        loop+=1;
        i+=2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
	return 'a';
}