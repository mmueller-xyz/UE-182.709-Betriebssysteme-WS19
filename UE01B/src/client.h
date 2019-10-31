#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_

int urlinfo(char* url, char* scheme, char* hostname, char* args);
char* urlfilename(char* url);
int request(char* url, int port, FILE* outfile);


#endif // CLIENT_H_
