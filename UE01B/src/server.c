/**
 * @file server.c
 * @author Maximilian Müller (11810852)
 * @date 31.10.2019
 *
 * @brief A simple HTTP Server
 * @details Creates an HTTP Server which handles GET request. 
 * The documentroot has to be passed.
 * The default file to be transmitted when a folder is requested is called
 * indexfile and can be specified with the -i argument (default: index.html).
 * The Port to bind to can be specified with -p (default: 8080)
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

#include "server.h"

#define MAX_CONNECTIONS 10
#define RFC822 "%a, %d %b %Y %H:%M:%S GMT"

volatile sig_atomic_t quit = 0;

static char* pname;

/**
 * @details Sets global variable 'quit' to 1 so the programm can safely close after all connections have been served
 * 
 * @param signal the singal beeing handeled
 */
void handle_soft_exit(int signal) {
    quit = 1;
}

/**
 * @brief Prints the correct usage of the Programm
 * 
 * @param pname the programmname (argv[0])
 * @return always returns EXIT_FAILURE
 */
static void usage(char* pname) {
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n"
    "\t-p PORT to bind to (default = 8080)\n"
    "\t-i filename of file, which is transmitted when a folder is requested\n", pname);
	exit(EXIT_FAILURE);
}


/**
 * @brief Handles a singular Connection beteen the server and a client
 * 
 * @param sockfile pointer to the socket
 * @param documentroot the servers root folder in respect to the html documents (conventionally /var/www)
 * @return returns EXIT_SUCCESS if connection was handeled sucessfully
 */
int handle_connection(FILE* sockfile, char* documentroot, char* indexfile) {
    char *buf = NULL;
    size_t len = 0;

    /* fetch first line in request */
    if (!getline(&buf, &len, sockfile)) return EXIT_FAILURE; 
    fprintf(stdout, "%s: Request: %s", pname, buf);
    
    /* extract method, path and protocoll from request */
    char* method = strtok(buf, " ");
    char* path = strtok(NULL, " ");
    char* protocol = strtok(NULL, "\r");

    /* decline request if the three parameters couldn't be extracted */
    if (!method||!path||!protocol) {
        fprintf(stderr, "%s: Bad Request\n", pname);
        send_error(sockfile, 400, "Bad Request");
        return EXIT_FAILURE;
    }
    
    if (strcmp(method, "GET")!=0) { /* decline request if method is not "GET" */
        fprintf(stderr, "%s: Not Implemented\n", pname);
        send_error(sockfile, 501, "Not Implemented");
        return EXIT_FAILURE;
    }

    /* create document path from documentroot, the path from the request and the (optional) indexfile */
    char *filepath = calloc(strlen(documentroot) +
        strlen(path) + strlen(indexfile) + 1, sizeof(char));
    strcpy(filepath, documentroot);
    strcat(filepath, path);
    if (filepath[strlen(filepath)-1]=='/')
        strcat(filepath, indexfile);
    
    fprintf(stdout, "%s: Requested File: %s\n", pname, filepath);
    
    while (strlen(buf)>2) {
        getline(&buf, &len, sockfile);
    }
    fprintf(stderr, "%s: Sending response\n", pname);

    send_file(sockfile, filepath);
    free(filepath);
    return EXIT_SUCCESS;
}

/**
 * @brief Creates, binds, and accepts a server-socket and handles http requests.
 * @details Creates, binds, and accepts a server-socket. When bound sucessfully, 
 * a loop is entered which ends if SIGINT or SIGTERM signals are recieved. This 
 * is done by emploing a singnal handeler 'handle_soft_exit' which sets the global
 * flag 'quit' to 1 when either signal is recieved. 
 * When inside the loop, it sequentially accepts connections from clients and 
 * serves them the requested file.
 * 
 * @param port The TCP/IP Port the server binds to
 * @param documentroot Path of the directory that requested files are relative to
 * @param indexfile The default file that is sent back, when a directory is requested
 * @return Returns EXIT_SUCCESS when the main loop is exited by 
 */
int http_server(char* port, char* documentroot, char* indexfile) {
    /* Set signal handeler */
    struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	sa.sa_handler = handle_soft_exit;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    int res = getaddrinfo(NULL, port, &hints, &ai);
    if (res!=0) {
        fprintf(stderr, "%s: Failed to get addrinfo: %s", pname, gai_strerror(res));
        return(EXIT_FAILURE);
    }

    int sockfd = -1;
    errno = EINTR;
    while ((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) <0) {
        if (errno != EINTR) {
            fprintf(stderr, "%s: Failed to create socket: %s\n", pname, strerror(errno));
            freeaddrinfo(ai);
            return(EXIT_FAILURE);
        }
    }

    while (bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
        if (errno != EINTR) {
            fprintf(stderr, "%s: Failed to bind socket: %s\n", pname, strerror(errno));
            freeaddrinfo(ai);
            return(EXIT_FAILURE);
        }
    }

    int optval = 1;
    while (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval)<0) {
        if (errno != EINTR) {
            fprintf(stderr, "%s: Failed to set socket options: %s\n", pname, strerror(errno));
            freeaddrinfo(ai);
            return(EXIT_FAILURE);
        }
    }

    while (listen(sockfd, MAX_CONNECTIONS)) {
        if (errno != EINTR) {
            fprintf(stderr, "%s: Failed to listen on port: %s\n", pname, strerror(errno));
            freeaddrinfo(ai);
            return(EXIT_FAILURE);
        }
    }

    fprintf(stdout, "%s: Created HTTP Server listening on Port: %s\n", pname, port);
    fprintf(stdout, "%s: Document Root:%s; Indexfile: %s\n", pname, documentroot, indexfile);

    FILE* sockfile;
    int connfd;
    struct sockaddr_in in_addr; //optional
    socklen_t in_addr_len = sizeof(struct sockaddr_in); //optional
    while (!quit) {
        if ((connfd = accept(sockfd, (struct sockaddr *) &in_addr, &in_addr_len))<0){
            fprintf(stderr, "%s: Failed to accept: %s\n", pname, strerror(errno));
            quit = 1;
        } else
        {
            // fprintf(stdout, "%s: Accepted connection %i\n", pname, in_addr.sin_addr.s_addr);
            sockfile = fdopen(connfd, "r+");
            handle_connection(sockfile, documentroot, indexfile);
            fclose(sockfile);
        }        
    }

    fprintf(stdout, "%s: Closing Server and exiting\n", pname);
    freeaddrinfo(ai);

    return(EXIT_SUCCESS);
}

/**
 * @brief Sends HTTP Error Message
 * 
 * @param sockfile pointer to the socket
 * @param scode HTTP Statuscode
 * @param sname HTTP Statuscode Name
 * @return
 */
int send_error(FILE* sockfile, int scode, char* sname) {
    int len = 71 + 2*3 + 2*strlen(sname); // 71 fixed Chars + 2x statuscode(3B) + 2xstatuscodename
    send_header(sockfile, scode, sname, len);
    fprintf(sockfile, "<html><head><title>%i %s</title></head>", scode, sname);
    fprintf(sockfile, "<body><p><b>%i:</b> %s</p></body></html>", scode, sname);
    fflush(sockfile);
    return EXIT_SUCCESS;
}

/**
 * @brief Sends HTTP Header including date, content-type and content-lenght
 * 
 * @param sockfile pointer to the socket

 * @param scode HTTP Statuscode
 * @param sname HTTP Statuscode Name
 * @param content_len Length of Following file in Bytes
 * @return
 */
int send_header(FILE* sockfile, int scode, char* sname, long content_len) {
    time_t now = time(NULL);
    char timebuf[128];
    strftime(timebuf, sizeof(timebuf), RFC822, gmtime(&now));

    fprintf(sockfile, "HTTP/1.1 %i %s\r\n", scode, sname);
    fprintf(sockfile, "Date: %s\r\n", timebuf);
    // fprintf(sockfile, "Content-type: text/html\r\n");
    fprintf(sockfile, "Content-Length: %li\r\n", content_len);
    fprintf(sockfile, "Connection: close\r\n");
    fprintf(sockfile, "\r\n");

    return EXIT_SUCCESS;
}

/**
 * @brief Sends HTTP response with file
 * 
 * @param sockfile pointer to the socket
 * @param filepath filepath of the requested file
 * @return
 */
int send_file(FILE* sockfile, char* filepath) {
    FILE* webpage;

    while ((webpage=fopen(filepath, "r"))==NULL) {
        if (errno != EINTR) {
            fprintf(stderr, "%s: File not found\n", pname);
            send_error(sockfile, 404, "Not Found");
            return EXIT_FAILURE;
        }
    }
    
    /* get filelength */
    fseek(webpage, 0, SEEK_END);
    long content_len = ftell(webpage);
    fseek(webpage, 0, SEEK_SET);
    
    send_header(sockfile, 200, "OK", content_len);

    int chars_read;
    // char *data = malloc(content_len);
    // while ((chars_read = fread(data, 1, content_len, webpage))>0)    
    //     fwrite(data, 1, chars_read, sockfile);
    // free(data);
    char data[2048];
    while ((chars_read = fread(data, 1, 2048, webpage)) > 0)    
        fwrite(data, 1, chars_read, sockfile);

    fclose(webpage);
    fflush(sockfile);
    return EXIT_SUCCESS;
}

/**
 * @brief Handles argument parsing from the commandline
 * 
 * @param argc Argument Counter
 * @param argv Argument Vector
 * @return
 */
int main (int argc, char **argv){
    pname = argv[0];

    /* Argument Parsing */
	char *p_arg = NULL, *i_arg = NULL, *indexfile, *docroot, *port;
	int opt_p = 0, opt_i = 0, c;

	while((c=getopt(argc, argv, "p:i:")) != -1) {
 		switch (c){
 			case 'p':// port
 				opt_p++;
				p_arg = optarg;
 				break;
 			case 'i':// index file
 				opt_i++;
				i_arg = optarg;
 				break;

 			case '?':
 			default:// illegal arguments
 				usage(argv[0]);
 				break;
 		}
 	}
 	if (opt_p > 1 || opt_i > 1) {
 		fprintf(stderr, "%s:, Every option can only be used unce!\n", pname);
 		usage(argv[0]);
 	}

	if (opt_p==0) port = "8080";
	else port = p_arg;

    if (opt_i==0) indexfile = "index.html";
    else indexfile = i_arg;

    if (argc-optind!=1) {
        fprintf(stderr, "%s: A Document Root has to be specified\n", pname);
        usage(argv[0]);
    }

    docroot = argv[optind];

    return(http_server(port, docroot, indexfile));
}
