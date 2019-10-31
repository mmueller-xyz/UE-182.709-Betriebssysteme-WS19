#ifndef ISOPALINDROM_H_   /* Include guard */
#define ISOPALINDROM_H_

// static void usage(void);

void check_palindrom(FILE **infiles, int in_len, FILE *outfile, int ignore_case, int ignore_whitespaces);
int is_palindrom(char *str, int ignore_case, int ignore_whitespaces); 

int main(int argc, char *argv[]);


#endif // ISOPALINDROM_H_
