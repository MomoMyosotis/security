#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

#define KEY_SIZE 128
#define BLOCK_SIZE 128

void encryptAES(const unsigned char *plaintext, int plaintext_len, const unsigned char *key, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) exit(EXIT_FAILURE);

    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);

    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
}

void decryptAES(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) exit(EXIT_FAILURE);

    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);

    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
}

void printHex(const unsigned char *data, int len) {
    for (int i = 0; i < len; ++i) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void printKey(const unsigned char *key) {
    printf("Generated key: ");
    printHex(key, KEY_SIZE / 8);
}

int main() {
    int choice;
    unsigned char key[KEY_SIZE / 8];
    unsigned char plaintext[BLOCK_SIZE / 8];
    unsigned char ciphertext[BLOCK_SIZE / 8];
    unsigned char decrypted[BLOCK_SIZE / 8];

    printf("Enter 1 to encrypt, 2 to decrypt: ");
    scanf("%d", &choice);

    if (choice == 1) {
        // Resto del codice invariato

        printf("Enter the ciphertext to decrypt: ");
        fflush(stdout); // Assicura che il prompt venga visualizzato prima dell'input
        fgets(ciphertext, sizeof(ciphertext), stdin);
        ciphertext[strlen(ciphertext) - 1] = '\0'; // Rimuove il newline finale

        decryptAES(ciphertext, strlen(ciphertext), key, decrypted);
        printf("Decrypted text: %s\n", decrypted);
    }

    return 0;
}