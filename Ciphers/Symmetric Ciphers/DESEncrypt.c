#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Modules/DESModules.c"
#include "../Modules/DESEncodeData.c"
#include "./DESKeyGen.c"

void encodeProcessorPermuteMatrixInitializer(permuteMatrix *IPInvM, permuteMatrix *EMM, permuteMatrix *PMM) {
    IPInvM->Column = MAXMATSIZE; IPInvM->Row = MAXMATSIZE;
    PCCopier(IPInvM, IPInv);

    EMM->Column = 6; EMM->Row = MAXMATSIZE;
    PCCopier(EMM,EM);

    PMM->Column = 4; PMM->Row = MAXMATSIZE;
    PCCopier(PMM,PM);
}

void encodeMessageIPMInitializer(permuteMatrix *IPM) {
    IPM->Column = MAXMATSIZE; IPM->Row = MAXMATSIZE;
    PCCopier(IPM, IP);
}

void binaryKeyToHexadecimalResult(char message[MAXKEYLENGTH+1], char result[ROUNDS+1]) {
    char group4bit[4+1];
    memset(result,0, ROUNDS);
    int groupInsertPointer = 0, resultPointer=0;

    for (int i = 0; i < MAXKEYLENGTH; i++) {
        group4bit[groupInsertPointer] = message[i];

        groupInsertPointer++;
        if (groupInsertPointer == 4) {  // End of group
            group4bit[4] = '\0';  // Null-terminate

            result[resultPointer]=binHexToHexGroup(group4bit);
            groupInsertPointer = 0;
            resultPointer++;
        }
    }
    result[ROUNDS]=0;
}

void messageGroupsMerger8(group6bit groups[MAXMATSIZE], char *output) {
    strcpy(output, groups[0].value);

    for (int i=1; i< MAXMATSIZE; i++) {
        messageHalvesMerger(output, groups[i].value, output);
    }
}

void breakToGroups8g(char message[KEYSIZE+1], group6bit groups[MAXMATSIZE]) {
    int groupPointer = 0, insertPointer = 0;
    char mappedRowNo[SBOXROW+1] = {0}, mappedColNo[SBOXCOLUMN+1] = {0};

    for (int i = 0; i < KEYSIZE; i++) {
        groups[groupPointer].value[insertPointer] = message[i];
        if (insertPointer == 0 || insertPointer == SBOXCOLUMN+1) {
            mappedRowNo[insertPointer == 0 ? 0 : 1] = message[i];
        } else if (insertPointer > 0 && insertPointer < SBOXCOLUMN+1) {
            mappedColNo[insertPointer - 1] = message[i];
        }

        insertPointer++;
        if (insertPointer == 6) {  // End of group
            groups[groupPointer].value[6] = '\0';  // Null-terminate

            groups[groupPointer].rowNo = binToDec2bit(mappedRowNo);
            groups[groupPointer].colNo = binToDec4bit(mappedColNo);

            groupPointer++;
            insertPointer = 0;

            // Clearing the temporary variables
            memset(mappedRowNo, 0, sizeof(mappedRowNo));
            memset(mappedColNo, 0, sizeof(mappedColNo));
        }
    }
}

void sBoxUsage(group6bit groups[MAXMATSIZE], int verbose) {
    int returnedS;
    for (int i = 0; i < MAXMATSIZE; i++) {
        returnedS=sBoxes[i].S[groups[i].rowNo][groups[i].colNo];
        strcpy(groups[i].value, binary[returnedS]);

        // Print S boxed Output
            if (verbose) {
                printf("\"%s\", ", groups[i].value);
            }
    }

    // For Consistent output
        if (verbose) {
            printf("\n");
        }
}

void processor(char L[ROUNDS+1][SPLITMESSAGELENGTH+1], char R[ROUNDS+1][SPLITMESSAGELENGTH+1], char keys[ROUNDS+1][PERMUTEDLENGTH+1], char *result, int verbose) {
    permuteMatrix IPInvM, EMM, PMM;
    encodeProcessorPermuteMatrixInitializer(&IPInvM, &EMM, &PMM);
    char firstXOR[KEYSIZE + 1] = {0}, extendedR[KEYSIZE + 1] = {0}, sMerged[SPLITMESSAGELENGTH+1], func[PMM.Row * PMM.Column +1];
    char intermediateSwappedResult[MAXKEYLENGTH+1];
    group6bit groups[MAXMATSIZE] = {0};

    for (int i = 1; i <= ROUNDS; i++) {
        // 🌿 Derive New L from Old R: 32 bit
        strcpy(L[i],R[i-1]);

        // 🌿 Extend the old R bit length: 48 bit
        permutedString(R[i - 1], extendedR, EMM);

        // 🌿 XOR the Extended Old R with new Key
        messageXOR(extendedR, keys[i - 1], firstXOR);
        
        // 🌿 Break the Message to 8 groups of 6 bit each for S box input Preparation: 8X6
        breakToGroups8g(firstXOR, groups);

        // Title to the S boxed Output
            if (verbose) {
                printf("\n Sboxed Groups: ");
            }
        // 🌿 Use the S boxes to Substitute the values: 8X4= 32bit
        sBoxUsage(groups, verbose);

        // 🌿 Merge the messages back from 8 groups to a string: 32 bit
        messageGroupsMerger8(groups, sMerged);

        // 🌿 Use Permuation Matrix P on the SBoxed outputs to get the functional value: 8X4=32 bit
        permutedString(sMerged, func, PMM);

        // 🌿 XOR Old L with the functional falue to get the new R
        messageXOR(L[i-1], func, R[i]);
        
        // Internal Variables checks
            if (verbose) {
                printf("\n\nRound %d", i);
                printf("\n L[%d]:", i);
                stringPrinter(L[i], 4);

                printf("\n R[%d]:", i);
                stringPrinter(R[i], 4);

                printf("\n Expanded R[%d]:", i-1);
                stringPrinter(extendedR, 6);

                printf("\n XOR Result:", i);
                stringPrinter(firstXOR, 6);

                printf("\n Groups: ");
                for (int k=0; k < MAXMATSIZE; k++) {
                    printf("\"%s [%d][%d]\", ", groups[k].value, groups[k].rowNo, groups[k].colNo);
                }

                printf(" sMerged: ");
                stringPrinter(sMerged, SBOXCOLUMN);

                printf("\n function value: ");
                stringPrinter(func,SBOXCOLUMN);
            }

        // 🌿 On the last round 16, swap the order of L and R from LR to RL 
        if (i==ROUNDS) {
            messageHalvesMerger(R[i], L[i], intermediateSwappedResult);
            permutedString(intermediateSwappedResult, result, IPInvM);
        }
    }
}


void encryptor(char *cipherOutput, int verbose, int keysVerbose) {
    permuteMatrix IPM;
    char keys[ROUNDS+1][PERMUTEDLENGTH+1];
    char L[ROUNDS+1][SPLITMESSAGELENGTH+1], R[ROUNDS+1][SPLITMESSAGELENGTH+1];
    char *binaryMessage=malloc((MAXKEYLENGTH+1) * sizeof(char)), *binaryMessagePlus=malloc(MAXKEYLENGTH+1 * sizeof(char));
    char binaryResult[MAXKEYLENGTH+1];

    // 🌿 Initialize the IP Permute Matrix 8X8 matrix
    encodeMessageIPMInitializer(&IPM);

    // 🌿 Generate the Keys necessary for encoding: 48 bits each
    keyGenerator(keys,verbose, keysVerbose); 

    // 🌿 Convert the message to binary: 64 bits 
    plainTextToMessage(messagePlain, binaryMessage);

    // Prints the Binarry form of the message
        if (verbose || keysVerbose) {
            printf("\n\n");
        }
        printf("M:");
        stringPrinter(binaryMessage, 4);

    // 🌿 Initial Permuation with the IP Permute Matrix: 64 bit
    permutedString(binaryMessage, binaryMessagePlus, IPM);

    // 🌿 Split the message to Left half and the Right half: 32 bit
    messageHalfSplitter(binaryMessagePlus, L[0], R[0]);

    // ▶️ 🌿 Actual Worker
    processor(L, R, keys, binaryResult, verbose);

    // Internal Variable Checking
        if (verbose) {
            printf("\n IP:");
            stringPrinter(binaryMessagePlus, 4);

            printf("\n L[0]:");
            stringPrinter(L[0], 4);
            printf("\n R[0]:");
            stringPrinter(R[0], 4);
            
            printf("\n\n Result: ");
            stringPrinter(binaryResult,4);
        }
    
    // 🌿 Convert binary Cipher to Hexadecimal Cipher
    binaryKeyToHexadecimalResult(binaryResult, cipherOutput);
}

int main() {
    int verbose =0, keysVerbose=1;
    char *cipherOutput;

    encryptor(cipherOutput, verbose, keysVerbose);

    printf("\nHexResult:%s\n", cipherOutput);
    return 0;
}