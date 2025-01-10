#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLENGTH 100

void transpositer(char input[100], char output[100], int shiftunit) {
    char normalized, add3, modded, restored;
    for (int i=0; i < strlen(input); i++) {
        if (input[i]>='a' && input[i]<='z') {
            normalized = input[i]-'a';
            add3 = normalized+shiftunit;
            modded = add3%26;
            restored = modded+'A';
            output[i] = restored;
        } else if (input[i]>='A' && input[i]<='Z') {
            normalized = input[i]-'A';
            add3 = normalized+shiftunit;
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
    char plainText[MAXLENGTH+1] = {0}, cipherText[MAXLENGTH+1] = {0}, shiftInput[3];
    int shiftUnit=0;

    printf(" Enter the number of shifts: ");
    // Using fgets because scanf and fgets don't mix that very well
    fgets(shiftInput, sizeof(shiftInput), stdin);
    shiftUnit = atoi(shiftInput); // Convert input to an integer

    printf("\n Enter the Plain Message: ");
    fgets(plainText, sizeof(plainText), stdin);
    plainText[strcspn(plainText, "\n")] = 0;

    transpositer(plainText, cipherText, shiftUnit);
    printf("\n Cipher Tex: %s\n", cipherText);
}