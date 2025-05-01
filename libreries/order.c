#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100

// Funzione per controllare se un file esiste
int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}

int main() {
    FILE *input_file;
    char word[MAX_WORD_LENGTH];
    char filename[20];
    FILE *output_file;
    
    // Apri il file libreria.txt in modalità lettura
    input_file = fopen("passwords.txt", "r");
    if (input_file == NULL) {
        printf("Errore nell'aprire il file libreria.txt\n");
        return 1;
    }
    
    // Leggi parola per parola
    while (fscanf(input_file, "%99s", word) != EOF) {
        // Rimuove eventuali caratteri non alfabetici (opzionale)
        int i = 0, j = 0;
        char clean_word[MAX_WORD_LENGTH] = {0};
        while (word[i]) {
            if (isalpha(word[i])) {
                clean_word[j++] = word[i];
            }
            i++;
        }

        // Determina la lunghezza della parola
        int word_length = strlen(clean_word);
        
        // Salta le parole vuote o con lunghezza zero
        if (word_length == 0) {
            continue;
        }

        // Costruisce il nome del file in base alla lunghezza della parola
        sprintf(filename, "p%d.txt", word_length);
        
        // Controlla se il file esiste, e se no lo crea
        if (!file_exists(filename)) {
            output_file = fopen(filename, "w");  // crea il file se non esiste
        } else {
            output_file = fopen(filename, "a");  // append al file esistente
        }

        if (output_file == NULL) {
            printf("Errore nell'aprire o creare il file %s\n", filename);
            fclose(input_file);
            return 1;
        }

        // Scrive la parola nel file
        fprintf(output_file, "%s\n", clean_word);

        // Chiudi il file di output
        fclose(output_file);
    }
    
    // Chiudi il file di input
    fclose(input_file);

    printf("Processo completato con successo.\n");

    return 0;
}
