#define MAXKEYLENGTH 65
#define MAXMATSIZE 8

typedef struct {
    int PC[MAXMATSIZE][MAXMATSIZE];
    int Row; 
    int Column;
} permuteMatrix;

typedef struct {
    int S[4][16];
} sBox;

char *hexToBinGroup(char hexKey) {
    switch (hexKey) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default:
            fprintf(stderr, "[ERR 01] Invalid hex key: %c\n", hexKey);
            return NULL;
    }
}

void stringPrinter(char *string, int delimiter) {
    if (string == NULL) return;
    for (int i = 0; i < strlen(string); i++) {
        if (delimiter != 0 && i % delimiter == 0) {
            printf(" ");
        }
        printf("%c", string[i]);
    }
}

void PCCopier(permuteMatrix *PM, int source[][MAXMATSIZE]) {
    for (int i = 0; i < PM->Row; i++) {
        for (int j = 0; j < PM->Column; j++) {
            PM->PC[i][j] = source[i][j];
        }
    }
}

void primaryKeyAppender(char *output, char *binaryGroup) {
    if (output == NULL || binaryGroup == NULL) return;
    int pointer = strlen(output);
    for (int i = 0; i < 4; i++) {
        output[pointer++] = binaryGroup[i];
    }
    output[pointer] = 0; 
}