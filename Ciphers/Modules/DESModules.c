#define MAXKEYLENGTH 64
#define SPLITMESSAGELENGTH 32
#define PERMUTEDLENGTH 56
#define SPLITKEYLENGTH 28
#define KEYSIZE 48
#define ROUNDS 16
#define MAXMATSIZE 8
#define SBOXCOLUMN 4
#define SBOXROW 2

typedef struct {
    int PC[MAXMATSIZE][MAXMATSIZE];
    int Row; 
    int Column;
} permuteMatrix;

typedef struct {
    int S[4][16];
} sBox;

typedef struct {
    char value[6+1];
    int rowNo;
    int colNo;
} group6bit;


char hex[16] = "0123456789ABCDEF";
char binary[16][5] = {
    "0000", "0001", "0010", "0011",
    "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011",
    "1100", "1101", "1110", "1111"
};

char *newHexToBinGroup(char hexKey) {
    for (int i=0; i < 16; i++) {
        if (hexKey==hex[i]) {
            return binary[i];
        }
    }   
    fprintf(stderr, "[ERR 01] Invalid hex key: %c\n", hexKey);
    return "X";
}

char binHexToHexGroup(char binHexKey[4+1]) {
    for (int i=0; i < 16; i++) {
        if (strcmp(binHexKey, binary[i])==0) {
            return hex[i];
        }
    }   
    fprintf(stderr, "[ERR 01] Invalid binary hex key: %s\n", binHexKey);
    return 'X';
}


// For S Boxes Indexing
int binToDec2bit(char binKey[2+1]) {
    for (int i=0; i < 4; i++) {
        if (binKey[0]==binary[i][2] && binKey[1]==binary[i][3]) {
            return i;
        }
    }
    fprintf(stderr, "[ERR 01] Invalid 2bit bin key: %s\n", binKey);
    return 23;
}

int binToDec4bit(char binKey[4+1]) {
    for (int i=0; i < 16; i++) {
        if(strcmp(binKey, binary[i])==0) {
            return i;
        }
    }
    fprintf(stderr, "[ERR 01] Invalid 4bit bin key: %s\n", binKey);
    return 23;
} 

// char *oldhexToBinGroup(char hexKey) {
//     switch (hexKey) {
//         case '0': return "0000";
//         case '1': return "0001";
//         case '2': return "0010";
//         case '3': return "0011";
//         case '4': return "0100";
//         case '5': return "0101";
//         case '6': return "0110";
//         case '7': return "0111";
//         case '8': return "1000";
//         case '9': return "1001";
//         case 'A': return "1010";
//         case 'B': return "1011";
//         case 'C': return "1100";
//         case 'D': return "1101";
//         case 'E': return "1110";
//         case 'F': return "1111";
//         default:
//             fprintf(stderr, "[ERR 01] Invalid hex key: %c\n", hexKey);
//             return NULL;
//     }
// }

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

void plainTextToMessage(char *plainText, char *output) {
    strcpy(output, "");
    for (int i = 0; i < strlen(plainText); i++) {
        primaryKeyAppender(output, newHexToBinGroup(plainText[i]));
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
    output[pointer]='\0';
}

void messageXOR(char *inputMessage1, char *inputMessage2, char *outputMessage) {
    strcpy(outputMessage, inputMessage2);

    if (!strcmp(outputMessage, inputMessage2)==0) {
        printf("\n[ERR03] Fatal Error, size mismatch\n");
        exit(1);
    }
    for (int i=0; i < strlen(inputMessage1); i++) {
        if (inputMessage1[i] == inputMessage2[i]) outputMessage[i]='0';
        else outputMessage[i]='1';
    }
    outputMessage[strlen(inputMessage1)]=0;
}