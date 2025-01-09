#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Modules/DESModules.c"
#include "./DESEncryptDecrypt.c"

#define MESSAGESIZE 100

void asciiToHex(char asciiChar, char hexOutput[2+1]) {
    sprintf(hexOutput, "%02X", (unsigned char)asciiChar);
    hexOutput[2] = '\0'; // Null-terminate the string
}

void hexMessageAppender(char *output, char hexGroup[2+1]) {
    if (output == NULL || hexGroup == NULL) return;
    static int pointer = 0;
    for (int i = 0; i < 2; i++) {
        output[pointer++] = hexGroup[i];
    }
    output[pointer] = 0; 
}

void hexToAscii(const char hexInput[3], char *test) {
    unsigned int asciiValue;
    sscanf(hexInput, "%2X", &asciiValue); // Parse the hexadecimal string to an integer
    *test = (char)asciiValue;
}

void asciiMessageToHexConverter(char asciiMessage[MESSAGESIZE+1], char hexOutput[2*MESSAGESIZE+1]) {
    char hexGroups[2 + 1];
    for (int i = 0; i < strlen(asciiMessage); i++) {
        asciiToHex(asciiMessage[i], hexGroups);
        hexMessageAppender(hexOutput, hexGroups);
    }
}

void hexMessageToASCIIConverter(char hexMessage[2 * MESSAGESIZE + 1], char asciiOutput[MESSAGESIZE + 1]) {
    char hexaGroups[2 + 1], asciiCharacter;
    int insertGroupPointer = 0, insertOutputPointer = 0;
    for (int i = 0; i < strlen(hexMessage); i++) {
        hexaGroups[insertGroupPointer] = hexMessage[i];
        insertGroupPointer++;

        if (insertGroupPointer == 2) {
            hexaGroups[2] = 0;
            insertGroupPointer = 0;

            if (strcmp(hexaGroups, "80") == 0) { // Stop processing padding
                break;
            }
            hexToAscii(hexaGroups, &asciiCharacter);
            asciiOutput[insertOutputPointer++] = asciiCharacter;
        }
    }
    asciiOutput[insertOutputPointer] = 0; // Null-terminate the ASCII output
}

void hexMessageParser(char hexMessage[2*MESSAGESIZE+1], char output[2*MESSAGESIZE+1], int verbose, int keysVerbose, int decrypt) {
    char hexMessageBlock[16+1]={0}, hexCipherBlock[16+1]={0};
    int insertBlockPointer=0;

    for (int i = 0; i < hexMessage[i]!=0; i++) {
        hexMessageBlock[insertBlockPointer]=hexMessage[i];
        insertBlockPointer++;

        if (insertBlockPointer==16) {
            hexMessageBlock[16]=0;
            insertBlockPointer=0;

            encryptorDecryptor(hexMessageBlock, hexCipherBlock, verbose, keysVerbose, decrypt);
            strcat(output, hexCipherBlock);
            memset(hexMessageBlock, 0, sizeof(hexMessageBlock));
        }
    }
    
    if (strlen(hexMessageBlock)!=0) {
        for (int i = strlen(hexMessageBlock); i < 16; i += 2) {
            hexMessageBlock[i] = '8';
            hexMessageBlock[i + 1] = '0';
        }

        hexMessageBlock[16]=0;

        encryptorDecryptor(hexMessageBlock, hexCipherBlock, verbose, keysVerbose, decrypt);
        strcat(output, hexCipherBlock);
    }
}

int main(){
    char userMessage[MESSAGESIZE+1]={0}, hexUserMessage[2*MESSAGESIZE+1]={0}, hexCipherMessage[2*MESSAGESIZE+1]={0};
    char returnedMessage[2*MESSAGESIZE+1]={0}, finalMessage[MESSAGESIZE+1]={0};
    int verbose =0, keysVerbose=0, decrypt=0;

    printf("Enter a string: ");
    fgets(userMessage, sizeof(userMessage), stdin);
    //userMessage[strcspn(userMessage, "\n")] = '\0'; // Remove trailing newline

    printf("\n Plain: %s\n", userMessage);

    asciiMessageToHexConverter(userMessage, hexUserMessage);
    printf("\n Hex Message: ");
    stringPrinter(hexUserMessage, 2);

    hexMessageParser(hexUserMessage, hexCipherMessage, verbose, keysVerbose, decrypt);
    printf("\n Cipher message: ");
    stringPrinter(hexCipherMessage, 2);

    decrypt = 1;
    hexMessageParser(hexCipherMessage, returnedMessage, verbose, keysVerbose, decrypt);
    printf("\n Returned Text: ");
    stringPrinter(returnedMessage,2);

    hexMessageToASCIIConverter(returnedMessage, finalMessage);
    printf("\n Final Text: %s", finalMessage);

    return 0;
}