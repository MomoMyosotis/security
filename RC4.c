#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 256

void swap(unsigned char *a, unsigned char *b) {
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}

// Key Scheduling Algorithm (KSA)
void KSA(unsigned char *key, int key_length, unsigned char *S) {
    int j = 0;

    for (int i = 0; i < N; i++) {
        S[i] = i;
    }

    for (int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % key_length]) % N;
        swap(&S[i], &S[j]);
    }
}

// Pseudo-Random Generation Algorithm (PRGA)
void PRGA(unsigned char *S, unsigned char *data, int data_length) {
    int i = 0, j = 0;

    for (int k = 0; k < data_length; k++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;
        swap(&S[i], &S[j]);
        unsigned char rnd = S[(S[i] + S[j]) % N];
        data[k] ^= rnd;
    }
}

void RC4(unsigned char *key, int key_length, unsigned char *data, int data_length) {
    unsigned char S[N];
    KSA(key, key_length, S);
    PRGA(S, data, data_length);
}

int main() {
    unsigned char key[] = "Key";
    unsigned char data[] = "Plaintext";

    int key_length = strlen((char *)key);
    int data_length = strlen((char *)data);

    printf("Original data: %s\n", data);

    // Encrypt the data
    RC4(key, key_length, data, data_length);
    printf("Encrypted data: ");
    for (int i = 0; i < data_length; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");

    // Decrypt the data (applying RC4 again will decrypt the data)
    RC4(key, key_length, data, data_length);
    printf("Decrypted data: %s\n", data);

    return 0;
}
