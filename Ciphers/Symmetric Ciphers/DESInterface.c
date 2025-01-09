#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Modules/DESModules.c"
#include "./DESEncryptDecrypt.c"

#define MESSAGESIZE 100
#define HEXENCODESIZE 2
#define HEXMESSAGEBLOCKSIZE 16

void asciiToHex(char asciiChar, char hexOutput[HEXENCODESIZE+1]) {
    sprintf(hexOutput, "%02X", (unsigned char)asciiChar);
    hexOutput[HEXENCODESIZE] = '\0'; // Null-terminate the string
}

void hexMessageAppender(char *output, char hexGroup[HEXENCODESIZE+1]) {
    if (output == NULL || hexGroup == NULL) return;
    static int pointer = 0;
    for (int i = 0; i < HEXENCODESIZE; i++) {
        output[pointer++] = hexGroup[i];
    }
    output[pointer] = 0; 
}

void hexToAscii(const char hexInput[HEXENCODESIZE+1], char *test) {
    unsigned int asciiValue;
    sscanf(hexInput, "%2X", &asciiValue); // Parse the hexadecimal string to an integer
    *test = (char)asciiValue;
}

void asciiMessageToHexConverter(char asciiMessage[MESSAGESIZE+1], char hexOutput[2*MESSAGESIZE+1]) {
    char hexGroups[HEXENCODESIZE + 1];
    for (int i = 0; i < strlen(asciiMessage); i++) {
        asciiToHex(asciiMessage[i], hexGroups);
        hexMessageAppender(hexOutput, hexGroups);
    }
}

void hexMessageToASCIIConverter(char hexMessage[2*MESSAGESIZE+1], char asciiOutput[MESSAGESIZE+1]) {
    char hexaGroups[HEXENCODESIZE + 1], asciiCharacter;
    int insertGroupPointer = 0, insertOutputPointer = 0;

    // 🌿 Divide into 2 digit hexadecimal blocks to be converted to ascii
    for (int i = 0; i < strlen(hexMessage); i++) {
        hexaGroups[insertGroupPointer] = hexMessage[i];
        insertGroupPointer++;

        if (insertGroupPointer == HEXENCODESIZE) {
            hexaGroups[HEXENCODESIZE] = 0;
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
    char hexMessageBlock[HEXMESSAGEBLOCKSIZE+1] = {0}, hexCipherBlock[HEXMESSAGEBLOCKSIZE+1] = {0};
    int insertBlockPointer = 0;

    // 🌿 Converting to 16 bit blocks to be fed into the encryptor decryptor function
    for (int i = 0; hexMessage[i] != 0; i++) {
        hexMessageBlock[insertBlockPointer] = hexMessage[i];
        insertBlockPointer++;

        if (insertBlockPointer == HEXMESSAGEBLOCKSIZE) {
            hexMessageBlock[HEXMESSAGEBLOCKSIZE] = 0;
            insertBlockPointer = 0;

            // 🌿 Feeding into the encryptor decryptor function 
            encryptorDecryptor(hexMessageBlock, hexCipherBlock, verbose, keysVerbose, decrypt);
            strcat(output, hexCipherBlock);
            memset(hexMessageBlock, 0, sizeof(hexMessageBlock));
        }
    }
    // 🌿 For Message that do not produce Hexadecimal Message of size Divisible by 16, use Filler 
    if (strlen(hexMessageBlock) != 0) {
        for (int i = strlen(hexMessageBlock); i < HEXMESSAGEBLOCKSIZE; i += HEXENCODESIZE) {
            hexMessageBlock[i] = '8';
            hexMessageBlock[i + 1] = '0';
        }

        hexMessageBlock[HEXMESSAGEBLOCKSIZE] = 0;

        encryptorDecryptor(hexMessageBlock, hexCipherBlock, verbose, keysVerbose, decrypt);
        strcat(output, hexCipherBlock);
    }
}

int main() {
    char userMessage[MESSAGESIZE+1]={0}, hexUserMessage[2*MESSAGESIZE+1]={0};
    char hexEncryptedMessage[2*MESSAGESIZE+1]={0}, encryptedMessage[MESSAGESIZE+1]={0};
    char hexDecryptedMessage[2*MESSAGESIZE+1]={0}, decryptedMessage[MESSAGESIZE+1]={0};
    int verbose = 0, keysVerbose = 0, decrypt = 0;

    // 🌿 Get The Plain Text (ascii) from user
    printf("Enter a string: ");
    fgets(userMessage, sizeof(userMessage), stdin);

    // Repeat the message
        if (verbose){
            printf("\n Plain Message: %s", userMessage);
        }
    
    // 1️⃣ ENCRYPTION 🌿 Convert to Hexadecimal by referencing the adcii decimal to 2 digit hexadecimal String
    asciiMessageToHexConverter(userMessage, hexUserMessage);
    // print the hexadecimal version of the message
        if (verbose) {
            printf("\n Hex Message: ");
            stringPrinter(hexUserMessage, HEXENCODESIZE);
        }

    // 🌿 Convert to Hexadecimal Cipher 
    hexMessageParser(hexUserMessage, hexEncryptedMessage, verbose, keysVerbose, decrypt);   

    // Print the cipher text
    printf("\n Hex Encrypted message: ");
    stringPrinter(hexEncryptedMessage, HEXENCODESIZE);
    // Commented becaue ascii version will look like junk in the terminal 
    //hexMessageToASCIIConverter(hexEncryptedMessage, encryptedMessage);
    //printf("\n Encrypted message: %s", encryptedMessage);

    // 2️⃣ DECRYPTION 🌿 Decrpting the message by refeeding it with the decrypt status bit ON
    decrypt = 1;
    hexMessageParser(hexEncryptedMessage, hexDecryptedMessage, verbose, keysVerbose, decrypt);
    if (verbose) {
        printf("\n Returned Hex Message: ");
        stringPrinter(hexDecryptedMessage, HEXENCODESIZE);
    }

    // 🌿 Convert the Hexadecimal Dencrypted text back to ascii
    hexMessageToASCIIConverter(hexDecryptedMessage, decryptedMessage);
    printf("\n Decrypted Message: %s", decryptedMessage);
    return 0;
}