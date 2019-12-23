#include "intmul.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void singMult(char A, char B) {
    int a = hextoint(A), b = hextoint(B);
    // printf("%c: %i\n%c: %i\n", A, a, B, b);
    if (a < 0 || b < 0) exit(EXIT_FAILURE);
    printf("%X", a*b);
}

int main(int argc, char const *argv[]) {
    char *A, *B, c = fgetc(stdin);
    int size = 2, i = 0;
    A = malloc(2);
    for(; c != EOF && c != '\n';c = fgetc(stdin)) {
        A[i++] = c;
        if (i >= size){
            size = size*size;
            A = realloc(A, size);
        }
    }
    size = i+1;
    A = realloc(A, size);
    A[i] = '\0';
    B = malloc(size);

    for (int j = 0; j < i ; j++){
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
    B[i] = '\0';
    
    // printf("A: %s\nB: %s\n", A, B);


    if (strlen(A) != strlen(B))
        exit(EXIT_FAILURE);

    if (i==1){
        singMult(A[0], B[0]);
    } else
    {
        if (i%2!=0) exit(EXIT_FAILURE);
        __pid_t pid[4];
        char *ABh, *AhB, *AlB, *ABl;
        int fd[4][2][2];

        pipe(fd[0][0]);
        pipe(fd[0][1]);
        // pipe(fd[1][0]);
        // pipe(fd[1][1]);
        // pipe(fd[2][0]);
        // pipe(fd[2][1]);
        // pipe(fd[3][0]);
        // pipe(fd[3][1]);

        for (int i = 0; i<4; i++) {
            pid[i] = fork();
            if (pid[i] == 0) {
                break;
            }
        }

        if (pid[0] == 0) {
            // closefd(fd, 0, 4);
            close(fd[0][0][1]);
            close(fd[0][1][0]);
            dup2(fd[0][0][0], STDIN_FILENO);
            dup2(fd[0][1][1], STDOUT_FILENO);
            // closefd(fd, -1, 4);
            execl(argv[0], (char *)NULL);
        } else if (pid[1] == 0)
        {
            // closefd(fd, 0, 4);
            // dup2(fd[1][0][0], STDIN_FILENO);
            // dup2(fd[1][1][1], STDOUT_FILENO);
            // closefd(fd, -1, 4);
            // execlp(argv[0], argv[0]);
        } else if (pid[2] == 0)
        {
            // closefd(fd, 0, 4);
            // dup2(fd[2][0][0], STDIN_FILENO);
            // dup2(fd[2][1][1], STDOUT_FILENO);
            // closefd(fd, -1, 4);
            // execlp(argv[0], argv[0]);
        } else if (pid[3] == 0)
        {
            // closefd(fd, 0, 4);
            // dup2(fd[3][0][0], STDIN_FILENO);
            // dup2(fd[3][1][1], STDOUT_FILENO);
            // closefd(fd, -1, 4);
            // execlp(argv[0], argv[0]);
            // exit(EXIT_FAILURE);
        } else
        {
            int i2 = i/2;
            char *Ah = malloc(i2+1), *Al = malloc(i2+1), *Bh = malloc(i2+1), *Bl = malloc(i2+1);
            strncpy(Ah, A, i2);
            strncpy(Al, A+i2, i2);
            strncpy(Bh, B, i2);
            strncpy(Bl, B+i2, i2);
            Ah[i2] = '\0';
            Al[i2] = '\0';
            Bh[i2] = '\0';
            Bl[i2] = '\0';
            
            close(fd[0][0][0]);
            close(fd[0][1][1]);
            FILE *out[4], *in[4];
            out[0] = fdopen(fd[0][0][1], "w");
            in[0] = fdopen(fd[0][1][0], "r");
            fprintf(out[0], "%s\n%s\n", Ah, Bh);
            fflush(out[0]);
            char buf[9];
            fgets(buf, 9, in[0]);
            printf("%s", buf);
            
            // fgets(buf, 8, fd[1][1][0]);
            // printf("%s", buf);
            // fgets(buf, 8, fd[2][1][0]);
            // printf("%s", buf);
            // fgets(buf, 8, fd[3][1][0]);
            // printf("%s", buf);
            // closefd(fd, -1, 4);

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
        }
    }
    

    return EXIT_SUCCESS;
}

void closefd(int*** fd, int i, int n) {
    for(int j = 0; j < n; j++) {
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
        return -1;
    }
}

