#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Modules/DESModules.c"
#include "../Modules/DESData.c"

#define ROUNDS 16

void permuteMatrixInitializer(permuteMatrix *PC1M, permuteMatrix *PC2M, permuteMatrix *IPM, permuteMatrix *IPInvM, permuteMatrix *EMM, permuteMatrix *PMM) {
    PC1M->Column = 7; PC1M->Row = 8;
    PCCopier(PC1M, PC1);

    PC2M->Column = 8; PC2M->Row = 6;
    PCCopier(PC2M, PC2);

    IPM->Column = 8; IPM->Row = 8;
    PCCopier(IPM, IP);

    IPInvM->Column = 8; IPInvM->Row = 8;
    PCCopier(IPInvM, IPInv);

    EMM->Column = 6; EMM->Row = 8;
    PCCopier(EMM, EM);
    
    PMM->Column = 4; PMM->Row = 8;
    PCCopier(PMM, PM);
}

void plainTextToMessage(message *output, char *plainText) {
    for (int i = 0; i < strlen(plainText); i++) {
        primaryKeyAppender(output, hexToBinGroup(plainText[i]));
    }
}

void permutedString(message input, message *output, permuteMatrix PM) {
    for (int i = 0; i < PM.Row; i++) {
        for (int j = 0; j < PM.Column; j++) {
            output->msg[output->pointer] = input.msg[PM.PC[i][j]-1];
            output->pointer++;
        }
    }
}

void messageSplitter(char *input, char *output1, char *output2) {
    if (strlen(input)%2==0) {
        for (int i=0; i < strlen(input); i++) {
            if (i < strlen(input)/2) {
                output1[i]=input[i];
            } 
            else if (i >= strlen(input)/2) {
                int normalizedi=i-(strlen(input)/2);
                output2[normalizedi]=input[i];
            }
        }
    }
    output1[strlen(input)/2]=0;
    output2[strlen(input)/2]=0;
}

void messageMerger(char *input1, char *input2, char *output) {
    strcpy(output, input1);
    strcat(output, input2);
    output[strlen(input1)+strlen(input2)]='\0';
}

void leftShiftCircular(char *input, int times) {
    for (int j=0; j<times; j++) {
        char hold = input[0];
        for (int i=0; i < strlen(input)-1; i++) {
            input[i] = input[i+1];
        }
        input[strlen(input)-1]=hold;
    }
}

void leftShiftCircularDual(char *input1, char *input2, int times) {
    leftShiftCircular(input1, times);
    leftShiftCircular(input2,times);
}

void scheduledLeftShifts(char C[ROUNDS][29], char D[ROUNDS][29]) {
    for (int i=1; i <= ROUNDS; i++) {
        strcpy(C[i], C[i-1]); 
        strcpy(D[i], D[i-1]);
        leftShiftCircularDual(C[i], D[i],rotationSchedule[i-1]);
        printf("\n C%-2d: %s\n D%-2d: %s",i, C[i],i, D[i]);
    }
}

void keyGenerator(char C[ROUNDS][29], char D[ROUNDS][29],char keys[ROUNDS][57]) {
    for (int i=1; i <= ROUNDS; i++) {
        messageMerger(C[i], D[i], keys[i-1]);
        printf("\n ShiftedMerged[%-2d]: %s",i, keys[i-1]);
    }
}

int main() {
    permuteMatrix PC1M, PC2M, IPM, EMM, IPInvM, PMM;
    permuteMatrixInitializer(&PC1M, &PC2M, &IPM, &IPInvM, &EMM, &PMM);

    char keyWord1[MAXKEYLENGTH] = "133457799BBCDEF1";

    // Allocate memory for the message structure
    message *binaryKey= malloc(sizeof(message));
    messageInitializer(binaryKey);
    message *binaryKeyplus = malloc(sizeof(message));;
    messageInitializer(binaryKeyplus);

    plainTextToMessage(binaryKey, keyWord1);
    printf("K=");
    stringPrinter(binaryKey->msg, 8);
    
    permutedString(*binaryKey, binaryKeyplus, PC1M);
    printf("K+=");
    stringPrinter(binaryKeyplus->msg, 7);

    char C[ROUNDS][29], D[ROUNDS][29], keys[ROUNDS][57], entire[57];
    messageSplitter(binaryKeyplus->msg, C[0], D[0]);
    printf("\n C0= %s \n D0= %s", C[0], D[0]);

    scheduledLeftShifts(C, D);
    keyGenerator(C, D, keys);
    return 0;
}