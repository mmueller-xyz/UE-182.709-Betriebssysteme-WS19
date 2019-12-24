#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int hextoint(char A);
static char inttohex(int i);
void singMult(char A, char B);
void closefd(int fd[4][2][2], int i);
char digitSum(char A, char B);
char digitCarry(char A, char B);
void add4(char* A, char* B, char* C, char* D, char* sum, int n);