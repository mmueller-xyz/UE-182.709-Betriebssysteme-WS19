/**
 * @file isopalindrom.c
 * @author Maximilian Müller
 * @date 22.10.2019
 *
 * @brief A Program, which checks whether strings are palindroms
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "isopalindrom.h"

/**
 * Prints the correct usage of the Programm
 * 
 * @param pname The path and name of the executed programm
 */
static void usage(char* pname) {
	fprintf(stderr, "Usage: %s [-s] [-i] [-o outfile] [file...]\n\t-s ignore whitespaces\n\t-i igonre upper/lowercase\n\t-o write output to specified file\n", pname);
	exit(EXIT_FAILURE);
}

/**
 * This function coordinates the checking of palindrom input sequences across the multiple input files.
 *
 * @param infiles this is an array of read-accessable files
 * @param in_len the number of input files
 * @param outfile this is an write-accessable file
 * @param ignore_case flag, indicating if casing should be ignored
 * @param ignore_whitespaces flag, indicating if whitespaces should be ignored
 */
void check_palindrom(FILE **infiles, int in_len, FILE *outfile, int ignore_case, int ignore_whitespaces) {
	char* str = NULL;
	size_t len = 0;
	for (int i = 0; i<in_len; i++) {
		while (getline(&str, &len, infiles[i]) != -1) {
			str[strcspn(str, "\n")] = 0; // remove any trailing carrage characters
			fprintf(outfile, "%s ", str);
			if (is_palindrom(str, ignore_case, ignore_whitespaces)) {
				fprintf(outfile, "is a palindrom\n");
			} else {
				fprintf(outfile, "is not a palindrom\n");
			}
			if (infiles[i]==stdin) fflush(outfile); // flush in case program gets terminated (<Ctrl+C> instead of <Ctrl+D>)
		}
	}
	fflush(outfile);
	//if (str) free(str);
}

/**
 * This function checks whether a given string is a palindrom,
 * meaning, that it is read the same forward and backwards
 *
 * @param str the input to be validated, it could be manipulated in the process
 * @param ignore_case if set to 0, upper case is ignored
 * @param ignore_whitespace if set to 0, all whitespaces are ignored
 * @return 1 if it is a palindrom, else it is not
 */
int is_palindrom(char *str, int ignore_case, int ignore_whitespaces) {
	if (ignore_whitespaces) {
		int j = 0;
		for (int i=0;str[i]; i++) {
			if (str[i] != ' ') str[j++] = str[i];
		}
		str[j] = 0;
	}
	if (ignore_case) for (int i =0; str[i]; i++) str[i] = tolower(str[i]);

	int i = 0, j = strlen(str) - 1;
	while (j > i) if (str[i++] != str[j--]) return(EXIT_SUCCESS);

	return(EXIT_FAILURE);
}

/**
 * Program entry point.
 *
 * @brief handles commandline arguments and creation of i/o files
 */
int main(int argc, char *argv[])
{
	/* Argument Parsing */
	char *o_arg = NULL;
	int opt_s = 0, opt_i = 0, opt_o = 0;
	int c;
	while((c=getopt(argc, argv, "sio:")) != -1) {
		switch (c){
			case 's':// option to ignore whitespaces
				opt_s++;
				break;
			case 'i':// option to ignore case
				opt_i++;
				break;
			case 'o':// option to set outfile
				opt_o++;
				o_arg = optarg;
				break;

			case '?':
			default:// illegal arguments
				usage(argv[0]);
				break;
		}
	}
	if (opt_s > 1 || opt_i > 1 || opt_o > 1) {
		fprintf(stderr, "Every option can only be used unce!\n");
		usage(argv[0]);
	} // option is repeated

	/* create i/o streams */
	FILE *outfile_fp, **infile_fp;
	if (o_arg != NULL) {
		outfile_fp = fopen(o_arg, "w");
		if (outfile_fp == NULL) {fprintf(stderr, "Can't open outfile '%s'!\n", o_arg);exit(1);}
	} else { outfile_fp = stdout; }

	/* generate a list of input file streams */
	int infiles = argc-optind;
	if (infiles > 0) {
		infile_fp = malloc(infiles * sizeof(FILE*));
		for (int i = 0; i < infiles; i++) {
			infile_fp[i] = fopen(argv[optind+i], "r");
			if (infile_fp[i] == NULL) {fprintf(stderr, "Can't open infile '%s'!\n", argv[optind+i]);exit(1);}
		}
	} else {
		infile_fp = malloc(sizeof(FILE*));
		infile_fp[0] = stdin;
		infiles = 1;
	}

	check_palindrom(infile_fp, infiles, outfile_fp, opt_i, opt_s);
	for (int i = 0; i<infiles; i++) {
		fclose(infile_fp[i]);
	}
	if (outfile_fp!=stdout) fclose(outfile_fp);
	if (infile_fp) free(infile_fp);
	return(EXIT_SUCCESS);
}
