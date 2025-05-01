#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 30
#define MAX_FILENAME_LENGTH 100
#define MAX_FILES 1000
#define HASH_SIZE 65536

typedef struct Node {
    char *word;
    struct Node *next;
} Node;

typedef struct {
    int total_words;
    int duplicates;
} Result;

Node *hash_table[HASH_SIZE];

// Funzione di hash
unsigned int hash(const char *word) {
    unsigned int hash = 0;
    while (*word) {
        hash = (hash << 5) + *word++;
    }
    return hash % HASH_SIZE;
}

// Funzione per controllare se una parola è già presente nella tabella hash
int check_duplicate(const char *word) {
    unsigned int index = hash(word);
    Node *current = hash_table[index];
    
    while (current) {
        if (strcmp(current->word, word) == 0) {
            return 1;  // Duplicato trovato
        }
        current = current->next;
    }
    
    // Non è un duplicato, aggiungilo
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        perror("Memoria insufficiente");
        exit(EXIT_FAILURE);
    }
    new_node->word = strdup(word);
    if (!new_node->word) {
        perror("Memoria insufficiente");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    new_node->next = hash_table[index];
    hash_table[index] = new_node;

    return 0;  // Non duplicato
}

// Funzione per liberare la memoria della tabella hash
void free_hash_table() {
    for (int i = 0; i < HASH_SIZE; i++) {
        Node *current = hash_table[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->word);
            free(temp);
        }
        hash_table[i] = NULL; // Reset della tabella
    }
}

// Funzione per processare un file
Result process_file(const char *input_filename) {
    FILE *input_file;
    FILE *output_file;
    char word[MAX_WORD_LENGTH + 1];
    int word_count = 0;
    int duplicates = 0;

    // Apri il file specificato in modalità lettura
    input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("Errore nell'aprire il file %s\n", input_filename);
        return (Result){0, 0}; // Restituisce zero se c'è un errore
    }

    // Crea un file temporaneo per le parole uniche
    char temp_filename[MAX_FILENAME_LENGTH];
    snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", input_filename);
    output_file = fopen(temp_filename, "w");
    if (output_file == NULL) {
        printf("Errore nella creazione del file temporaneo %s\n", temp_filename);
        fclose(input_file);
        return (Result){0, 0}; // Restituisce zero se c'è un errore
    }

    printf("Elaborazione del file: %s\n", input_filename);

    // Leggi parola per parola
    while (fscanf(input_file, "%30s", word) == 1) {
        // Rimuove eventuali caratteri non alfabetici
        int j = 0;
        char clean_word[MAX_WORD_LENGTH + 1] = {0};
        for (int i = 0; word[i] != '\0'; i++) {
            if (isalpha(word[i]) && j < MAX_WORD_LENGTH) {
                clean_word[j++] = word[i];
            }
        }
        clean_word[j] = '\0';

        // Salta le parole vuote o con lunghezza zero
        if (strlen(clean_word) == 0) {
            continue;
        }

        // Conta le parole totali
        word_count++;

        // Controlla per duplicati
        if (check_duplicate(clean_word)) {
            duplicates++;
        } else {
            fprintf(output_file, "%s\n", clean_word);
        }

        // Mostra il progresso
        printf("Confrontando parola: %s\n", clean_word);
    }

    fclose(input_file);
    fclose(output_file);

    // Sostituisci il file originale con quello temporaneo
    remove(input_filename);
    rename(temp_filename, input_filename);

    // Libera la memoria della tabella hash
    free_hash_table();

    // Restituisci i risultati
    return (Result){word_count, duplicates};
}

// Funzione per ottenere i file in un intervallo
void get_files_in_range(const char *directory, char files[][MAX_FILENAME_LENGTH], int *count) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, "p") == ent->d_name && strstr(ent->d_name, ".txt") != NULL) {
                strcpy(files[*count], ent->d_name);
                (*count)++;
            }
        }
        closedir(dir);
    } else {
        perror("Errore nell'aprire la directory");
    }
}

void process_files_in_range(char files[][MAX_FILENAME_LENGTH], int start_index, int end_index, int total_files) {
    Result results[MAX_FILES];

    for (int i = start_index; i <= end_index; i++) {
        // Mostra la barra di avanzamento
        printf("Elaborando %s (%d/%d)... ", files[i], i - start_index + 1, total_files);
        fflush(stdout);

        results[i] = process_file(files[i]);

        // Barra di avanzamento completa
        printf("Fatto!\n");
    }

    // Stampa i risultati alla fine
    printf("\nRiepilogo dei risultati:\n");
    for (int i = start_index; i <= end_index; i++) {
        printf("File: %s - Parole totali: %d, Ripetizioni eliminate: %d\n",
               files[i], results[i].total_words, results[i].duplicates);
    }
}

int main() {
    char choice;
    char files[MAX_FILES][MAX_FILENAME_LENGTH];
    int count = 0;

    // Ottieni i file dalla directory corrente
    get_files_in_range(".", files, &count);

    do {
        printf("File trovati:\n");
        for (int i = 0; i < count; i++) {
            printf("%d: %s\n", i + 1, files[i]);
        }

        printf("Inserisci il numero del file iniziale (1-%d): ", count);
        int start, end;
        scanf("%d", &start);
        printf("Inserisci il numero del file finale (1-%d): ", count);
        scanf("%d", &end);

        // Controllo dell'intervallo
        if (start < 1 || end < 1 || start > count || end > count || start > end) {
            printf("L'intervallo non è valido.\n");
            continue;
        }

        // Processa i file nell'intervallo
        process_files_in_range(files, start - 1, end - 1, count);

        printf("Vuoi controllare un altro intervallo? (c/e): ");
        scanf(" %c", &choice); // Nota lo spazio prima di %c per ignorare eventuali newline
    } while (choice == 'c');

    printf("Operazioni terminate. Grazie!\n");
    return 0;
}
