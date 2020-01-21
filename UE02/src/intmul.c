/**
 * @file intmul.c
 * @author Maximilian Müller
 * @date 23.12.2019
 *
 * @brief A Hex multiplier working with stdin and forks
 *
 * Some more detailed Comments
 */

#include "intmul.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * @brief multiplies two single HEX digits
 * 
 * @param A first digit
 * @param B second digit
 */
void singMult(char A, char B) {
    int a = hextoint(A), b = hextoint(B);
    // printf("%c: %i\n%c: %i\n", A, a, B, b);
    if (a < 0 || b < 0) exit(EXIT_FAILURE);
    printf("%X", a*b);
}

int main(int argc, char const *argv[]) {
    char *A, *B, c = fgetc(stdin);
    int size = 2, digits = 0;
    A = malloc(2);

    // read first HEX number
    for(; c != EOF && c != '\n';c = fgetc(stdin)) {
        A[digits++] = c;
        if (digits >= size){
            size = size*size;
            A = realloc(A, size);
        }
    }
    size = digits+1;
    A = realloc(A, size);
    A[digits] = '\0';
    B = malloc(size);

    // read second HEX number
    for (int j = 0; j < digits ; j++){
        c = fgetc(stdin);
        if (c != EOF && c != '\n'){
            B[j] = c;
        } else
        {
            exit(EXIT_FAILURE);
        }
    }
    c=fgetc(stdin);
    if (c != EOF && c != '\0' && c != '\n') exit(2);
    B[digits] = '\0';
    
    // printf("A: %s\nB: %s\n", A, B);


    if (strlen(A) != strlen(B))
        exit(EXIT_FAILURE);

    if (digits==1){
        singMult(A[0], B[0]);
    } else
    {
        if (digits%2!=0) exit(EXIT_FAILURE);
        __pid_t pid[4];
        int fd[4][2][2];

        pipe(fd[0][0]);
        pipe(fd[0][1]);
        pipe(fd[1][0]);
        pipe(fd[1][1]);
        pipe(fd[2][0]);
        pipe(fd[2][1]);
        pipe(fd[3][0]);
        pipe(fd[3][1]);

        for (int i = 0; i<4; i++) {
            pid[i] = fork();
            if (pid[i] == 0) {
                break;
            }
        }

        if (pid[0] == 0) {
            closefd(fd, 0);
            dup2(fd[0][0][0], STDIN_FILENO);
            dup2(fd[0][1][1], STDOUT_FILENO);
            execl(argv[0], argv[0], (char *)NULL);
        } else if (pid[1] == 0)
        {
            closefd(fd, 1);
            dup2(fd[1][0][0], STDIN_FILENO);
            dup2(fd[1][1][1], STDOUT_FILENO);
            execl(argv[0], argv[0], (char *)NULL);
        } else if (pid[2] == 0)
        {
            closefd(fd, 2);
            dup2(fd[2][0][0], STDIN_FILENO);
            dup2(fd[2][1][1], STDOUT_FILENO);
            execl(argv[0], argv[0], (char *)NULL);
        } else if (pid[3] == 0)
        {
            closefd(fd, 3);
            dup2(fd[3][0][0], STDIN_FILENO);
            dup2(fd[3][1][1], STDOUT_FILENO);
            execl(argv[0], argv[0], (char *)NULL);
        } else
        {
            int i2 = digits/2;
            char *Ah = malloc(i2+1), *Al = malloc(i2+1), *Bh = malloc(i2+1), *Bl = malloc(i2+1);
            strncpy(Ah, A, i2);
            strncpy(Al, A+i2, i2);
            strncpy(Bh, B, i2);
            strncpy(Bl, B+i2, i2);
            Ah[i2] = '\0';
            Al[i2] = '\0';
            Bh[i2] = '\0';
            Bl[i2] = '\0';

            FILE *out[4], *in[4];
            for (int i = 0; i<4; i++) {
                close(fd[i][0][0]);
                close(fd[i][1][1]);
                out[i] = fdopen(fd[i][0][1], "w");
                in[i] = fdopen(fd[i][1][0], "r");
            }

            // Write to stdin
            fprintf(out[0], "%s\n%s\n", Ah, Bh);
            fprintf(out[1], "%s\n%s\n", Ah, Bl);
            fprintf(out[2], "%s\n%s\n", Al, Bh);
            fprintf(out[3], "%s\n%s\n", Al, Bl);
            fflush(out[0]);
            fflush(out[1]);
            fflush(out[2]);
            fflush(out[3]);

            // wait for childs
            int status, num_ex = 0;
            while (num_ex < 4){
                if(wait(&status) != -1)
                    if (WEXITSTATUS(status) == EXIT_SUCCESS)
                        num_ex++;                        
                    else
                        {
                            printf("%i", WEXITSTATUS(status));
                        exit(EXIT_FAILURE);
                        }
                else
                    exit(EXIT_FAILURE);                
            }

            //get calculations from stdout
            char *ABh = malloc(digits*2+1), *AhB = malloc(digits*2+1), *AlB = malloc(digits*2+1), *ABl = malloc(digits*2+1);
            char *buf = malloc(size);
            for (int j = 0; j < digits*2; j++){
                ABh[j] = '0';
                AhB[j] = '0';
                AlB[j] = '0';
                ABl[j] = '0';
            }
            ABh[digits*2] = '\0';
            AhB[digits*2] = '\0';
            AlB[digits*2] = '\0';
            ABl[digits*2] = '\0';
            
            // padding for calculations
            fgets(buf, size, in[0]);
            strcpy(ABh+digits-strlen(buf), buf);
            ABh[digits] = '0';

            fgets(buf, size, in[1]);
            strcpy(AhB+digits+digits/2-strlen(buf), buf);
            AhB[digits+digits/2] = '0';

            fgets(buf, size, in[2]);
            strcpy(AlB+digits+digits/2-strlen(buf), buf);
            AlB[digits+digits/2] = '0';

            fgets(buf, size, in[3]);
            strcpy(ABl+2*digits-strlen(buf), buf);
            
            fprintf(stderr, "AhBh: %s\n", ABh);
            fprintf(stderr, "AhBl: %s\n", AhB);
            fprintf(stderr, "AlBh: %s\n", AlB);
            fprintf(stderr, "AlBl: %s\n", ABl);

            char *product = calloc(2*digits+1, 1);

            add4(ABh, AhB, AlB, ABl, product, digits*2);

            printf("%s\n", product);

            free(Ah);
            free(Bh);
            free(Al);
            free(Bl);
            free(ABh);
            free(AhB);
            free(AlB);
            free(ABl);
            free(buf);
            free(product);
        }
    }
    

    return EXIT_SUCCESS;
}

/**
 * @brief Adds two HEX numbers, carry can't go over the size and size must be equal
 * 
 * @param A first number
 * @param B second number
 * @param sum pointer to the sum
 * @param n number of digits in sum
 */
void add(char* A, char* B, char* sum, int n){
    char carry = '0';

    for (int i = n-1; i >= 0; i--){
        sum[i] = digitSum(digitSum(A[i], B[i]), carry);
        carry = digitSum(digitCarry(A[i], B[i]), digitCarry(digitSum(A[i], B[i]), carry));
    }    
}


/**
 * @brief Adds four HEX numbers, carry can't go over the size and size must be equal 
 * 
 * @param A first number
 * @param B second number
 * @param C third number
 * @param D forth number
 * @param sum pointer to the sum
 * @param n number of digits in sum
 */
void add4(char* A, char* B, char* C, char* D, char* sum, int n){
    char *sum_[2];
    sum_[0] = malloc(n+1);
    sum_[1] = malloc(n+1);
    add(A, B, sum_[0], n);
    fprintf(stderr, "%s + %s = %s\n", A, B, sum_[0]);
    add(C, D, sum_[1], n);
    fprintf(stderr, "%s + %s = %s\n", C, D, sum_[1]);
    add(sum_[0], sum_[1], sum, n);
    fprintf(stderr, "%s + %s = %s\n", sum_[0], sum_[1], sum);
}



/**
 * @brief returns the mod 16 of an addition (ie. one digit without carry)
 * 
 * @param A first digit
 * @param B second digit
 * @return the sum
 */
char digitSum(char A, char B) {
    int a = hextoint(A);
    int b = hextoint(B);
    return inttohex((a+b) & 0x0F);
}

/**
 * @brief returns the carry of an addition
 * 
 * @param A first digit
 * @param B second digit
 * @return the carry
 */
char digitCarry(char A, char B) {
    int a = hextoint(A);
    int b = hextoint(B);
    return inttohex((a+b) >> 4);
}


/**
 * @brief closes all pipes, that a child doesn't use
 * 
 * @param fd the pointer with all pipes
 * @param i the number of the child
 */
void closefd(int fd[4][2][2], int i) {
    for(int j = 0; j < 4; j++) {
        if (j == i) {
            close(fd[j][0][1]);
            close(fd[j][1][0]);
        } else
        {
            close(fd[j][0][0]);
            close(fd[j][0][1]);
            close(fd[j][1][0]);
            close(fd[j][1][1]);
        }
        
    }
}

/**
 * @brief converts an integer 0 <= i <= 0xF to a hex digit
 */
static char inttohex(int i) {
    switch (i) {
    case 0:
        return '0';
    case 1:
        return '1';
    case 2:
        return '2';
    case 3:
        return '3';
    case 4:
        return '4';
    case 5:
        return '5';
    case 6:
        return '6';
    case 7:
        return '7';
    case 8:
        return '8';
    case 9:
        return '9';
    case 10:
        return 'A';
    case 11:
        return 'B';
    case 12:
        return 'C';
    case 13:
        return 'D';
    case 14:
        return 'E';
    case 15:
        return 'F';
    default:
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief converts a hex digit to an integer
 */
static int hextoint(char A) {
    switch (A){
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'a':
    case 'A':
        return 10;
    case 'b':
    case 'B':
        return 11;
    case 'c':
    case 'C':
        return 12;
    case 'd':
    case 'D':
        return 13;
    case 'e':
    case 'E':
        return 14;
    case 'f':
    case 'F':
        return 15;
    default:
        exit(EXIT_FAILURE);
    }
}

