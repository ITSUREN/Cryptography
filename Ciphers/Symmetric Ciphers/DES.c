#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int PC1[8][7]={
    { 57, 49, 41, 33, 25, 17,  9},
    {  1, 58, 50, 42, 34, 26, 18},
    { 10,  2, 59, 51, 43, 35, 27},
    { 19, 11,  3, 60, 52, 44, 36},
    { 63, 55, 47, 39, 31, 23, 15},
    {  7, 62, 54, 46, 38, 30, 22},
    { 14,  6, 61, 53, 45, 37, 29},
    { 21, 13,  5, 28, 20, 12,  4}
};

int PC2[6][8]={
    { 14, 17, 11, 24,  1,  5,  3, 28},
    { 15,  6, 21, 10, 23, 19, 12,  4},
    { 26,  8, 16,  7, 27, 20, 13,  2},
    { 41, 52, 31, 37, 47, 55, 30, 40},
    { 51, 45, 33, 48, 44, 49, 39, 56},
    { 34, 53, 46, 42, 50, 36, 29, 32},
};

int IP[8][8]= {
    { 58, 50, 42, 34, 26, 18, 10, 2},
    { 60, 52, 44, 36, 28, 20, 12, 4},
    { 62, 54, 46, 38, 30, 22, 14, 6},
    { 64, 56, 48, 40, 32, 24, 16, 8},
    { 57, 49, 41, 33, 25, 17,  9, 1},
    { 59, 51, 43, 35, 27, 19, 11, 3},
    { 61, 53, 45, 37, 29, 21, 13, 5},
    { 63, 55, 47, 39, 31, 23, 15, 7}
};

int EM[8][6]= {
    { 32,  1,  2,  3,  4,  5},
    {  4,  5,  6,  7,  8,  9},
    {  8,  9, 10, 11, 12, 13},
    { 12, 13, 14, 15, 16, 17},
    { 16, 17, 18, 19, 20, 21},
    { 20, 21, 22, 23, 24, 25},
    { 24, 25, 26, 27, 28, 29},
    { 28, 29, 30, 31, 32,  1}
};

sBox sBoxes[8] = {
    {
        {
            {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
            {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
            {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
            {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
        }        
    },
    {
        {
            {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
            {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
            {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
            {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
        }
        
    },
    {
        {
            {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
            {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
            {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
            {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
        }
        
    },
    {
        {
            {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
            {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
            {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
            {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
        }
        
    },
    {
        {
            {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
            {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
            {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
            {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
        }
        
    },
    {
        {
            {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
            {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
            {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
            {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
        }
        
    },
    {
        {
            {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
            {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
            {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
            {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
        }
        
    },
    {
        {
            {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
            {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
            {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
            {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
        }
        
    },
};

void permuteMatrixInitializer(permuteMatrix *PC1M, permuteMatrix *PC2M, permuteMatrix *IP, permuteMatrix *EM) {
    memcpy(&(PC1M->PC), &PC1, sizeof(PC1));
    PC1M->Column=7; PC2M->Row=8;
    memcpy(&(PC2M->PC), &PC2, sizeof(PC2));
    PC2M->Column=8; PC2M->Row=6;
    memcpy(&(IP->PC), &IP, sizeof(IP));
    IP->Column=8; IP->Row=8;
    memcpy(&(EM->PC), &EM, sizeof(EM));
    EM->Column=6; EM->Row=8;
}

int rotationSchedule[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

char *hexToBin(char hexKey)  {
    switch(hexKey) {
        case '0':
            return "0000";
            break;
        case '1':
            return "0001";
            break;
        case '2':
            return "0010";
            break;
        case '3':
            return "0011";
            break;
        case '4':
            return "0100";
            break;
        case '5':
            return "0101";
            break;
        case '6':
            return "0110";
            break;
        case '7':
            return "0111";
            break;
        case '8':
            return "1000";
            break;
        case '9':
            return "1001";
            break;
        case 'A':
            return "1010";
            break;
        case 'B':
            return "1011";
            break;
        case 'C':
            return "1100";
            break;
        case 'D':
            return "1101";
            break;
        case 'E':
            return "1110";
            break;
        case 'F':
            return "1111";
            break;
        default:
            printf("\n[ERR 01] hexToBin function edge case encountered.");
    }
}

void binaryMessageAppender(message *output, char *binaryGroup) {
    for (int i=0; i < 4; i++) {
        output->msg[output->pointer] = binaryGroup[i];
        output->pointer++;
    }
    output->msg[output->pointer]='\0'; 
}

void plainTextToMessage(message *output, char *plainText) {
    static int firsttime=1;
    if (firsttime) {
        output->pointer=0;
        firsttime=0;
    }
    for (int i=0; i < strlen(plainText); i++) {
        binaryMessageAppender(output,hexToBin(plainText[i]));
    }
}

int main() {
    permuteMatrix PC1M,PC2M, IP, EM;
    permuteMatrixInitializer(&PC1M, &PC2M, &IP, &EM);

    char keyWord[MAXKEYLENGTH]="123A";
    message *output;

    plainTextToMessage(output,keyWord);
    printf("\nTEST: %s and %d\n", output->msg, sBoxes[1].S[0][0]);
}