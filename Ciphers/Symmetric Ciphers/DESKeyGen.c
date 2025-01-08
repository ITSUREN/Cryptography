#include "../Modules/DESKeyGenData.c"

void keyPermuteMatrixInitializer(permuteMatrix *PC1M, permuteMatrix *PC2M) {
    PC1M->Column = 7; PC1M->Row = 8;
    PCCopier(PC1M, PC1);

    PC2M->Column = 6; PC2M->Row = 8;
    PCCopier(PC2M, PC2);
}

void messageHalfSplitter(char *input, char *output1, char *output2) {
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

void messageHalvesMerger(char *input1, char *input2, char *output) {
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

void scheduledLeftShifts(char C[ROUNDS+1][SPLITKEYLENGTH+1], char D[ROUNDS+1][SPLITKEYLENGTH+1], int verbose) {
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

void permuteShiftedKeys(char C[ROUNDS+1][SPLITKEYLENGTH+1], char D[ROUNDS+1][SPLITKEYLENGTH+1],char keys[ROUNDS+1][PERMUTEDLENGTH+1], permuteMatrix *PC2M, int verbose, int keysVerbose) {
    char temp[ROUNDS+1][PERMUTEDLENGTH+1];
    for (int i=1; i <= ROUNDS; i++) {
        messageHalvesMerger(C[i], D[i], keys[i-1]);

        permutedString(keys[i-1], temp[i-1], *PC2M);

        strcpy(keys[i-1], temp[i-1]);

        if (keysVerbose) {
            printf("Keys[%02d]:",i);
            stringPrinter(keys[i-1], 6);
            printf("\n");
        }
    }
}

void keyGenerator(char keysOutput[ROUNDS+1][PERMUTEDLENGTH+1], int verbose, int keysVerbose) {
    permuteMatrix PC1M, PC2M;
    char C[ROUNDS+1][SPLITKEYLENGTH+1], D[ROUNDS+1][SPLITKEYLENGTH+1];
    char *binaryKey=malloc((MAXKEYLENGTH+1) * sizeof(char)), *binaryKeyPlus=malloc(MAXKEYLENGTH+1 * sizeof(char));

    // 🌿 Initialize the Arrays into the Data Structure
    keyPermuteMatrixInitializer(&PC1M, &PC2M);

    // 🌿 Convert the Hexadecimal Key to Binary Key Result=64bit
    plainTextToMessage(keyWord, binaryKey);

    // Print the key
        if (verbose) {
            printf("K=");
            stringPrinter(binaryKey, 8);
        }
    
    // 🌿 Permute the Key against PC1 Result=56bit
    permutedString(binaryKey, binaryKeyPlus, PC1M);

    // Print the permuted Key
        if (verbose) {
            printf("K+=");
            stringPrinter(binaryKeyPlus, PC1M.Column);
        }

    // 🌿 Divide the Key to C and D havles Result=28 bit each
    messageHalfSplitter(binaryKeyPlus, C[0], D[0]);

    // 🌿 Individually Circular Left shift on C and D
    scheduledLeftShifts(C, D, verbose);

    // 🌿 Merge and Permute the key finally against PC2 Res=48 bit
    permuteShiftedKeys(C, D, keysOutput, &PC2M, verbose, keysVerbose);

    // Freeing 
    free(binaryKey);
    free(binaryKeyPlus);
}