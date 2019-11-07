#ifndef SERVER_H_   /* Include guard */
#define SERVER_H_

int handle_connection(FILE* sockfile, char* documentroot, char* indexfile);
int send_file(FILE* sockfile, char* filepath);
int http_server(char* port, char* documentroot, char* indexfile);
int send_error(FILE* sockfile, int scode, char* sname);
int send_header(FILE* sockfile, int statuscode, char* statusname, long content_len);


#endif // SERVER_H_
