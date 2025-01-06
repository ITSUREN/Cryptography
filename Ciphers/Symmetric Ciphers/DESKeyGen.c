#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Modules/DESModules.c"
#include "../Modules/DESKeyGenData.c"

#define SPLITLENGTH 28
#define PERMUTEDLENGTH 56

void keyPermuteMatrixInitializer(permuteMatrix *PC1M, permuteMatrix *PC2M) {
    PC1M->Column = 7; PC1M->Row = 8;
    PCCopier(PC1M, PC1);

    PC2M->Column = 6; PC2M->Row = 8;
    PCCopier(PC2M, PC2);
}

void primaryKeyAppender(char *output, char *binaryGroup) {
    if (output == NULL || binaryGroup == NULL) return;
    int pointer = strlen(output);
    for (int i = 0; i < 4; i++) {
        output[pointer++] = binaryGroup[i];
    }
    output[pointer] = 0; 
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

void scheduledLeftShifts(char C[ROUNDS+1][SPLITLENGTH+1], char D[ROUNDS+1][SPLITLENGTH+1], int verbose) {
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
                printf("\n C%02d: ", i);
                stringPrinter(C[i],0);
                printf("\n D%02d: ", i);
                stringPrinter(D[i], 0);
            }
    }
    // Give a new line if verbose
        verbose? printf("\n"): (void)0;
}

void permuteShiftedKeys(char C[ROUNDS+1][SPLITLENGTH+1], char D[ROUNDS+1][SPLITLENGTH+1],char keys[ROUNDS+1][PERMUTEDLENGTH+1], permuteMatrix *PC2M, int verbose) {
    char temp[ROUNDS+1][PERMUTEDLENGTH+1];
    for (int i=1; i <= ROUNDS; i++) {
        messageMerger(C[i], D[i], keys[i-1]);

        permutedString(keys[i-1], temp[i-1], *PC2M);

        strcpy(keys[i-1], temp[i-1]);

        //if (verbose) {
            printf("Keys[%02d]:",i);
            stringPrinter(keys[i-1], 6);
            printf("\n");
        //}
    }
}

void keyGenerator(char keysOutput[ROUNDS+1][PERMUTEDLENGTH+1], int verbose) {
    permuteMatrix PC1M, PC2M;
    char C[ROUNDS+1][SPLITLENGTH+1], D[ROUNDS+1][SPLITLENGTH+1];
    char *binaryKey=malloc((MAXKEYLENGTH+1) * sizeof(char)), *binaryKeyplus=malloc(MAXKEYLENGTH+1 * sizeof(char));

    // 🌿 Initialize the Arrays into the Data Structure
    keyPermuteMatrixInitializer(&PC1M, &PC2M);

    // 🌿 Convert the Hexadecimal Key to Binary Key Result=64bit
    plainTextToMessage(binaryKey, keyWord);

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
            stringPrinter(binaryKeyplus, PC1M.Column);
        }

    // 🌿 Divide the Key to C and D havles Result=28 bit each
    messageSplitter(binaryKeyplus, C[0], D[0]);

    // 🌿 Individually Circular Left shift on C and D
    scheduledLeftShifts(C, D, verbose);

    // 🌿 Merge and Permute the key finally against PC2 Res=48 bit
    permuteShiftedKeys(C, D, keysOutput, &PC2M, verbose);

    // Freeing 
    free(binaryKey);
    free(binaryKeyplus);
}

int main() {
    int verbose =0;
    char keys[ROUNDS+1][PERMUTEDLENGTH+1];
    
    keyGenerator(keys,verbose); 
    return 0;
}