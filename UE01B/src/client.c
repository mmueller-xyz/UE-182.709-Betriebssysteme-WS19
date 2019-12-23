/**
 * @file client.c
 * @author Maximilian Müller
 * @date 28.10.2019
 *
 * @brief A simple HTTP client, able to send http requests and displays the respnes in a file
 *
 * Some more detailed Comments
 */

#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
//#include <ctype.h>
#include "client.h"
static void usage(char* pname);

static char* pname;

/**
 * @brief Prints the correct usage of the Programm
 */
static void usage(char* pname_) {
	fprintf(stderr, "%s: Usage: %s [-p PORT] [-o FILE | -d DIR] URL\n\t-p PORT to connect to (default = 80)\n\t-o write output to specified file\n\t-d directory to write response to\n", pname, pname_);
	exit(EXIT_FAILURE);
}

int request(char* url, char* port, FILE* outfile){

	int *p = urlinfo(url);
	//printf("%i, %i, %i\n", p[0], p[1], p[2]);
	char* hostname = malloc(strlen(url)+1);
	strncpy(hostname, url+p[0], p[1]-p[0]);
	// printf("%s\n", hostname);
	char* file = malloc(strlen(url)+10);
	strncpy(file, url+p[2], strlen(url)+1-p[2]);
	// printf("%s %s\n", hostname, file);

    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(hostname, port, &hints, &ai);
    if (res!=0) {
        fprintf(stderr, "%s: Failed to get addrinfo: %s", pname, gai_strerror(res));
        return(EXIT_FAILURE);
    }

    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) {
        fprintf(stderr, "%s: Failed to create socket: %s", pname, strerror(errno));
        freeaddrinfo(ai);
        return(EXIT_FAILURE);
    }

	if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
		fprintf(stderr, "%s: Failed to connect to socket: %s", pname, strerror(errno));
        freeaddrinfo(ai);
        return(EXIT_FAILURE);
	}

	FILE* sockfile = fdopen(sockfd, "r+");
	send_request(sockfile, hostname, file);

	receive_header(sockfile);

	int chars_read;
    char data[2048];
    while ((chars_read = fread(data, 1, 2048, sockfile)) > 0)    
		fwrite(data, 1, chars_read, outfile);

	freeaddrinfo(ai);
	close(sockfd);
	return(EXIT_SUCCESS);
}

void send_request(FILE* sockfile, char* host, char* file) {
	int pntr = 0;
	char* buf = calloc(1, strlen(host)+strlen(file)+100);
	pntr = sprintf(buf, "GET %s HTTP/1.1\r\n", file);
	pntr += sprintf(buf+pntr, "Host: %s\r\n", host);
	pntr += sprintf(buf+pntr, "Connection: close\r\n\r\n");
	fprintf(sockfile, "%s", buf);
	fflush(sockfile);
	free(buf);
}

void receive_header(FILE* sockfile) {
	char* buf = NULL;
	size_t len = 0;

	if (!getline(&buf, &len, sockfile)) {
		fprintf(stderr, "%s: Protocol error!\n", pname);
		exit(2);
	}
	char* method = strtok(buf, " ");
    char* statuscode = strtok(NULL, " ");
    char* statusmsg = strtok(NULL, "\r");

	char* ptr;
	strtol(statuscode, &ptr, 10);
	if (strcmp(method, "HTTP/1.1")!=0 || strcmp(ptr, statuscode)==0) {
		fprintf(stderr, "%s: Protocol error!\n", pname);
		exit(2);
	}

	if (atol(statuscode)!=200) {
		fprintf(stderr, "%s: %s %s\n", pname, statuscode, statusmsg);
		exit(3);
	}

	while (strlen(buf)>2) {
        getline(&buf, &len, sockfile);
    }
}

/**
 * @brief splits an URL into the Scheme, Host, and other Arguments
 *
 * @param url the URL to analyze
 * @return the first array entry has the position of the first character from the Host
 *		   the second array entry has the position of the first character from the junk between Hostname and Arguments
 *		   the third array entry has the position of the first character from the Arguments
 */
int* urlinfo(char* url) {
	static char* host_term = ";/?:@=&";
	static int pos[3];

	/* Find first char of hostname */
	pos[0] = 0; 
	while (++pos[0] < strlen(url)-1)
		if (url[pos[0]-1] == '/' && url[pos[0]] == '/')
			break;
	pos[0]++;
	
	/* Find first char of junk between hostname and file */
	pos[1] = pos[0];
	for (int h = 1; h; pos[1]++){
		for (int j = 0; j <= strlen(host_term); j++)
			/* <= because of the \0 char */
			if (url[pos[1]] == host_term[j]) 
				h=0;
	}
	pos[1]--;

	/* Find first char of File */
	pos[2] = pos[1];
	for (int h = 1; h; pos[2]++){
		if (url[pos[2]] == '/' || url[pos[2]] == '\0') 
			h=0;
	}
	pos[2]--;


	if (strlen(url) <= 1) {
		pos[0] = strlen(url);
		pos[1] = strlen(url);
		pos[2] = strlen(url);
	}

	return pos;
}

/**
 * Program entry point.
 *
 * @param
 * @return Returns EXIT_SUCCESS
 */
 int main(int argc, char *argv[]) {
	pname = argv[0];

	/* Argument Parsing */
	char *p_arg = NULL, *o_arg = NULL, *d_arg = NULL, *url, *port;
	int opt_p = 0, opt_o = 0, opt_d = 0, c;

	while((c=getopt(argc, argv, "p:o:d:")) != -1) {
 		switch (c){
 			case 'p':// option to ignore whitespaces
 				opt_p++;
				p_arg = optarg;
 				break;
 			case 'o':// option to ignore case
 				opt_o++;
				o_arg = optarg;
 				break;
 			case 'd':// option to set outfile
 				opt_d++;
 				d_arg = optarg;
 				break;

 			case '?':
 			default:// illegal arguments
 				usage(argv[0]);
 				break;
 		}
 	}
 	if (opt_p > 1 || opt_o > 1 || opt_d > 1) {
 		fprintf(stderr, "%s: Every option can only be used unce!\n", pname);
 		usage(argv[0]);
 	} // option is repeated

	if (opt_o > 0 && opt_d > 0) {
		fprintf(stderr, "%s: You can either choose a file or a dir!\n", pname);
		usage(argv[0]);
	} 

	if ((argc-optind)!=1) {
		fprintf(stderr, "%s: You have to specify ONE URL!\n", pname);
		usage(argv[0]);
	}

	if (opt_p==0) port = "80";
	else port = p_arg;

	url = argv[optind];

	FILE* outfile;
	if (opt_d) {
		int* p = urlinfo(url);
		char* fn = malloc(strlen(url)+10);
		strncpy(fn, url+p[2], strlen(url)+1-p[2]);
		strcat(fn, "index.html");

		char *path = malloc(sizeof(char)*(strlen(fn)+strlen(d_arg)+1));
		strcpy(path, d_arg);
		strcat(path, fn);
		if ((outfile = fopen(path, "w")) == NULL) {
			fprintf(stderr, "%s: Could not open file: %s\n", pname, path);
			exit(EXIT_FAILURE);
		}
		free(path);
	} else if (opt_o) {
		if ((outfile = fopen(o_arg, "w")) == NULL) {
			fprintf(stderr, "%s: Could not open file: %s\n", pname, o_arg);
			exit(EXIT_FAILURE);
		}

	} else outfile=stdout;


	// fprintf(stderr, "%s: URL: %s, PORT: %s\n", pname, url, port);
	request(url, port, outfile);

	fclose(outfile);
	return EXIT_SUCCESS;
}
