#ifndef STDIO_H
#define STDIO_H
    #include <stdio.h>
#endif
#ifndef STDLIB_H
#define STDLIB_H
    #include <stdlib.h>
#endif

#define MAXKEYLENGTH 64
#define MAXMATSIZE 8

typedef struct {
    char *msg;
    int pointer;
} message;

typedef struct {
    int PC[MAXMATSIZE][MAXMATSIZE];
    int Row; 
    int Column;
} permuteMatrix;

typedef struct {
    int S[4][16];
} sBox;

char *hexToBinGroup(char hexKey) {
    switch (hexKey) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default:
            printf("\n[ERR 01] hexToBin function edge case encountered.");
            return NULL;
    }
}

void stringPrinter(char *string, int delimeter) {
    for (int i=0; i<strlen(string); i++) {
        if (i%delimeter==0) {
            printf(" ");
        }
        printf("%c", string[i]);
    }
    printf("\n");
}

void PCCopier(permuteMatrix *PM, int source[][MAXMATSIZE]) {
    for (int i = 0; i < PM->Row; i++) {
        for (int j = 0; j < PM->Column; j++) {
            PM->PC[i][j] = source[i][j];
        }
    }
}

void primaryKeyAppender(message *output, char *binaryGroup) {
    for (int i = 0; i < 4; i++) {
        output->msg[output->pointer] = binaryGroup[i];
        output->pointer++;
    }
    output->msg[output->pointer] = '\0'; 
}

void messageInitializer(message *msg) {
    msg->msg = malloc(64*sizeof(int)+1);
    msg->pointer = 0;
}

