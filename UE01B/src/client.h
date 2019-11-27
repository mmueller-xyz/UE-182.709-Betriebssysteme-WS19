#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_

int* urlinfo(char* url);
int request(char* url, char* port, FILE* outfile);
void send_request(FILE* sockfile, char* host, char* file);
void receive_header(FILE* sockfile);

#endif // CLIENT_H_
