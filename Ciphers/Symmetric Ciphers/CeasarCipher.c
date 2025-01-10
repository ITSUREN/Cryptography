#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLENGTH 100
#define SHIFTUNIT 3

void ceasarCipher(char input[MAXLENGTH], char output[MAXLENGTH]) {
    char normalized, add3, modded, restored;
    for (int i=0; i < strlen(input); i++) {
        if (input[i]>='a' && input[i]<='z') {
            normalized = input[i]-'a';
            add3 = normalized+SHIFTUNIT;
            modded = add3%26;
            restored = modded+'A';
            output[i] = restored;
        } else if (input[i]>='A' && input[i]<='Z') {
            normalized = input[i]-'A';
            add3 = normalized+SHIFTUNIT;
            modded = add3%26;
            restored = modded+'A';
            output[i] = restored;
        } else if (input[i] == ' ') {
            output[i] = input[i];
        } else {
            output[i] = '?';
        }
    }
}

int main() {
    char plainText[MAXLENGTH+1] = {0}, cipherText[MAXLENGTH+1] = {0};

    printf(" Enter the Plain Message: ");
    fgets(plainText, sizeof(plainText), stdin);
    plainText[strcspn(plainText, "\n")] = 0;

    ceasarCipher(plainText, cipherText);
    printf("\n Cipher Tex: %s\n", cipherText);
}