#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 5

void toUpperCase(char *str);
void removeDuplicates(char *str);
void generateKeySquare(char key[], char keySquare[SIZE][SIZE]);
void prepareText(char *text);
void digraphize(char *text, char *preparedText);
void playfairEncrypt(char preparedText[], char keySquare[SIZE][SIZE], char encryptedText[]);
void findPosition(char ch, char keySquare[SIZE][SIZE], int *row, int *col);

int main() {
    char key[SIZE * SIZE];
    char keySquare[SIZE][SIZE];
    char text[100], preparedText[100], encryptedText[100];

    printf("Enter the key: ");
    fgets(key, sizeof(key), stdin);
    key[strcspn(key, "\n")] = '\0';

    printf("Enter the plaintext: ");
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = '\0';

    toUpperCase(key);
    removeDuplicates(key);
    generateKeySquare(key, keySquare);

    toUpperCase(text);
    prepareText(text);
    digraphize(text, preparedText);

    playfairEncrypt(preparedText, keySquare, encryptedText);

    printf("Encrypted text: %s\n", encryptedText);

    return 0;
}

void toUpperCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

void removeDuplicates(char *str) {
    int index = 0;
    int n = strlen(str);

    for (int i = 0; i < n; i++) {
        int j;
        for (j = 0; j < i; j++) {
            if (str[i] == str[j]) {
                break;
            }
        }
        if (j == i) {
            str[index++] = str[i];
        }
    }
    str[index] = '\0';
}

void generateKeySquare(char key[], char keySquare[SIZE][SIZE]) {
    int keyLength = strlen(key);
    int used[26] = {0};
    int row = 0, col = 0;

    for (int i = 0; i < keyLength; i++) {
        if (key[i] == 'J') {
            key[i] = 'I';
        }
        if (!used[key[i] - 'A']) {
            keySquare[row][col++] = key[i];
            used[key[i] - 'A'] = 1;
            if (col == SIZE) {
                col = 0;
                row++;
            }
        }
    }

    for (char ch = 'A'; ch <= 'Z'; ch++) {
        if (ch == 'J') continue;
        if (!used[ch - 'A']) {
            keySquare[row][col++] = ch;
            used[ch - 'A'] = 1;
            if (col == SIZE) {
                col = 0;
                row++;
            }
        }
    }
}

void prepareText(char *text) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        if (text[i] == 'J') {
            text[i] = 'I';
        }
    }
}

void digraphize(char *text, char *preparedText) {
    int len = strlen(text);
    int index = 0;
    for (int i = 0; i < len; i += 2) {
        preparedText[index++] = text[i];
        if (i + 1 < len && text[i] == text[i + 1]) {
            preparedText[index++] = 'X';
            i--;
        } else if (i + 1 < len) {
            preparedText[index++] = text[i + 1];
        } else {
            preparedText[index++] = 'X';
        }
    }
    preparedText[index] = '\0';
}

void playfairEncrypt(char preparedText[], char keySquare[SIZE][SIZE], char encryptedText[]) {
    int len = strlen(preparedText);
    int row1, col1, row2, col2;
    for (int i = 0; i < len; i += 2) {
        findPosition(preparedText[i], keySquare, &row1, &col1);
        findPosition(preparedText[i + 1], keySquare, &row2, &col2);

        if (row1 == row2) {
            encryptedText[i] = keySquare[row1][(col1 + 1) % SIZE];
            encryptedText[i + 1] = keySquare[row2][(col2 + 1) % SIZE];
        } else if (col1 == col2) {
            encryptedText[i] = keySquare[(row1 + 1) % SIZE][col1];
            encryptedText[i + 1] = keySquare[(row2 + 1) % SIZE][col2];
        } else {
            encryptedText[i] = keySquare[row1][col2];
            encryptedText[i + 1] = keySquare[row2][col1];
        }
    }
    encryptedText[len] = '\0';
}

void findPosition(char ch, char keySquare[SIZE][SIZE], int *row, int *col) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (keySquare[r][c] == ch) {
                *row = r;
                *col = c;
                return;
            }
        }
    }
}
