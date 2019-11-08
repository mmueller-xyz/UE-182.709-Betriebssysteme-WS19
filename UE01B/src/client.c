/**
 * @file client.c
 * @author Maximilian Müller
 * @date 28.10.2019
 *
 * @brief A simple HTTP client, able to send http requests and displays the respnes in a file
 *
 * Some more detailed Comments
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
//#include <ctype.h>
#include "client.h"
static void usage(char* pname);

static char* pname;

/**
 * Prints the correct usage of the Programm
 */
static void usage(char* pname_) {
	fprintf(stderr, "%s: Usage: %s [-p PORT] [-o FILE | -d DIR] URL\n\t-p PORT to connect to (default = 80)\n\t-o write output to specified file\n\t-d directory to write response to\n", pname, pname_);
	exit(EXIT_FAILURE);
}

int request(char* url, int port, FILE* outfile){

//	char *scheme = malloc();

	return(EXIT_SUCCESS);
}

/**
 * splits an URL into the Scheme, Host, and other Arguments
 *
 * @param url the URL to analyze
 * @param scheme a pointer to a string where the scheme is written to
 * @param hostname a pointer to a string where the hostname is written to
 * @param args a pointer to a string where the arguments are
 */
int urlinfo(char* url, char* scheme, char* hostname, char* args) {



	return(EXIT_SUCCESS);
}

/**
 * Extracts the filename according to the following regular expression:
 * /[^/]*$/
 * ie: "foo/bar" evaluates to "bar"
 * and "foo/"    evaluates to ""
 * @param url a URL string,
 * @return filename of the url
 */
char* urlfilename(char* url) {
	size_t start_pos = strlen(url);
	while (url[start_pos-1] != '/' && start_pos>0) start_pos--;

	size_t filename_size = strlen(url)-start_pos;
	char* filename = malloc(sizeof(char)*filename_size);

	for (size_t i = 0;i<filename_size;i++)
		filename[i] = url[start_pos+i];

	//printf("%s\n%s\n", url, filename);
	return (filename);
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
	char *p_arg = NULL, *o_arg = NULL, *d_arg = NULL, *url;
	int opt_p = 0, opt_o = 0, opt_d = 0, c, port;

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

	if (opt_p==0) port = 80;
	else port = atoi(p_arg);

	url = argv[optind];

	FILE* outfile;
	if (opt_d) {
		char *fn = urlfilename(url);
		char *path = malloc(sizeof(char)*(strlen(fn)+strlen(d_arg)+1));
		strcpy(path, d_arg);
		strcat(path, fn);
		if ((outfile = fopen(path, "w")) == NULL) {
			fprintf(stderr, "%s: Could not open file: %s\n", pname, path);
			exit(EXIT_FAILURE);
		}
		free(path);
	} if (opt_o) {
		if ((outfile = fopen(o_arg, "w")) == NULL) {
			fprintf(stderr, "%s: Could not open file: %s\n", pname, o_arg);
			exit(EXIT_FAILURE);
		}

	} else outfile=stdout;


	printf("%s: URL: %s, PORT: %i\n", pname, url, port);
	request(url, port, outfile);

	fclose(outfile);
	return EXIT_SUCCESS;
}
