#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Modules/DESModules.c"
#include "../Modules/DESData.c"

#define ROUNDS 16

void keypermuteMatrixInitializer(permuteMatrix *PC1M, permuteMatrix *PC2M) {
    PC1M->Column = 7; PC1M->Row = 8;
    PCCopier(PC1M, PC1);

    PC2M->Column = 6; PC2M->Row = 8;
    PCCopier(PC2M, PC2);
}

void plainTextToMessage(char *output, char *plainText) {
    strcpy(output, "");
    for (int i = 0; i < strlen(plainText); i++) {
        primaryKeyAppender(output, hexToBinGroup(plainText[i]));
    }
}

void permutedString(char *input, char *output, permuteMatrix PM) {
    int pointer =0;
    for (int i = 0; i < PM.Row; i++) {
        for (int j = 0; j < PM.Column; j++) {
            output[pointer] = input[PM.PC[i][j]-1];
            pointer++;
        }
    }
    output[pointer]=0;
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

void scheduledLeftShifts(char C[ROUNDS+1][29], char D[ROUNDS+1][29], int verbose) {
    // Print the First Key that does not require permuting
        if (verbose) {
            printf("\n C0 : %s \n D0 : %s", C[0], D[0]);
        }
        
    for (int i=1; i <= ROUNDS; i++) {
        strcpy(C[i], C[i-1]); 
        strcpy(D[i], D[i-1]);
        leftShiftCircularDual(C[i], D[i],rotationSchedule[i-1]);

        // To print the split Key
            if (verbose) {
                printf("\n C%-2d: ", i);
                stringPrinter(C[i],0);
                printf("\n D%-2d: ", i);
                stringPrinter(D[i], 0);
            }
    }
    // Give a new line if verbose
        verbose? printf("\n"): (void)0;
}

void permuteShiftedKeys(char C[ROUNDS+1][29], char D[ROUNDS+1][29],char keys[ROUNDS+1][57], permuteMatrix *PC2M, int verbose) {
    char temp[ROUNDS+1][57];
    for (int i=1; i <= ROUNDS; i++) {
        messageMerger(C[i], D[i], keys[i-1]);

        permutedString(keys[i-1], temp[i-1], *PC2M);

        strcpy(keys[i-1], temp[i-1]);

        //if (verbose) {
            printf("Keys[%-2d]:",i);
            stringPrinter(keys[i-1], 6);
            printf("\n");
        //}
    }
}

void keyGenerator(char keyWords12[MAXKEYLENGTH], int verbose) {
    permuteMatrix PC1M, PC2M;
    char C[ROUNDS+1][29], D[ROUNDS+1][29], keys[ROUNDS+1][57];
    char *binaryKey=malloc(MAXKEYLENGTH * sizeof(char)), *binaryKeyplus=malloc(MAXKEYLENGTH * sizeof(char));

    // 🌿 Initialize the Arrays into the Data Structure
    keypermuteMatrixInitializer(&PC1M, &PC2M);

    // 🌿 Convert the Hexadecimal Key to Binary Key Result=64bit
    plainTextToMessage(binaryKey, keyWords12);

    // Print the key
        if (verbose) {
            printf("K=");
            stringPrinter(binaryKey, 8);
        }
    
    // 🌿 Permute the Key against PC1 Result=56bit
    permutedString(binaryKey, binaryKeyplus, PC1M);

    // Print the permuted Key
        if (verbose) {
            printf("K+=");
            stringPrinter(binaryKeyplus, 7);
        }

    // 🌿 Divide the Key to C and D havles Result=28 bit each
    messageSplitter(binaryKeyplus, C[0], D[0]);

    // 🌿 Individually Circular Left shift on C and D
    scheduledLeftShifts(C, D, verbose);

    // 🌿 Merge and Permute the key finally against PC2 Res=48 bit
    permuteShiftedKeys(C, D, keys, &PC2M, verbose);

    // Freeing 
    free(binaryKey);
    free(binaryKeyplus);
}

int main() {
    char keyWord123[MAXKEYLENGTH]="133457799BBCDEF1";
    int verbose =0;
    
    keyGenerator(keyWord123, verbose); 
    return 0;
}