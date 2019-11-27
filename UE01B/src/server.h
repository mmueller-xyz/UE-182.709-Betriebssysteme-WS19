/**
 * @file server.h
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
#ifndef SERVER_H_   /* Include guard */
#define SERVER_H_

int handle_connection(FILE* sockfile, char* documentroot, char* indexfile);
int send_file(FILE* sockfile, char* filepath);
int http_server(char* port, char* documentroot, char* indexfile);
int send_error(FILE* sockfile, int scode, char* sname);
int send_header(FILE* sockfile, int statuscode, char* statusname, long content_len);


#endif // SERVER_H_
