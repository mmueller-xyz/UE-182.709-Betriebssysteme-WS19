/**
 * @file client.h
 * @author Maximilian Müller
 * @date 28.10.2019
 *
 * @brief A simple HTTP client, able to send http requests and displays the respnes in a file
 *
 * Some more detailed Comments
 */
#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_

int* urlinfo(char* url);
int request(char* url, char* port, FILE* outfile);
void send_request(FILE* sockfile, char* host, char* file);
void receive_header(FILE* sockfile);

#endif // CLIENT_H_
