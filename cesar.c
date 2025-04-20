#include <stdio.h>
#include <string.h>
#include <ctype.h>

int calculateKey(char *key) {
    int sum = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        sum += key[i];
    }
    return sum;
}

void caesarCipher(char *text, int shift) {
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (isalpha(c)) {  // Controlla se il carattere è una lettera
            char offset = isupper(c) ? 'A' : 'a';  // Determina l'offset per maiuscole/minuscole
            c = (c - offset + shift) % 26 + offset;  // Applica lo spostamento e normalizza
        }
        text[i] = c;  // Aggiorna il carattere codificato
    }
}

int main() {
    char key[256];
    char text[256];
    int confirm;

    // Chiede la parola chiave all'utente
    do {
        printf("Insert key word for Caesar's cipher: ");
        scanf("%s", key);

        // Calcola la somma dei valori ASCII delle lettere della parola chiave
        int shift = calculateKey(key);

        // Mostra la somma calcolata e chiede conferma
        printf("\nThe given key is: %s (sum of ASCII values: %d).\nAnswer only with '0' or '1'\nConfirm? ", key, shift);
        scanf("%d", &confirm);

        if (confirm != 0 && confirm != 1) {
            printf("\nInvalid input. Terminating...\n");
            return 1;  // Termina il programma se l'input è invalido
        }
        if (confirm == 0) {
            printf("Okay, we'll try again.\n");
        }
    } while (confirm != 1);

    // Chiede la frase da cifrare
    printf("Enter the phrase to encrypt: ");
    getchar();  // Consuma il newline rimasto nel buffer
    fgets(text, sizeof(text), stdin);

    // Rimuove il carattere di newline dalla stringa, se presente
    text[strcspn(text, "\n")] = '\0';

    // Cifra la frase usando il cifrario di Cesare
    caesarCipher(text, calculateKey(key));

    // Stampa la frase cifrata
    printf("Encrypted phrase: %s\n", text);

    return 0;
}