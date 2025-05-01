#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_LENGTH 30
#define MAX_PROCESSES 10
#define CHUNK_SIZE 5

// Funzione per rimuovere caratteri non alfanumerici
void cleanString(char *str) {
    int i, j = 0;
    char temp[MAX_LENGTH];
    for (i = 0; str[i] != '\0'; i++) {
        if (isalnum(str[i])) {
            temp[j++] = str[i];
        }
    }
    temp[j] = '\0';
    strcpy(str, temp);
}

// Funzione per controllare se una password è già stata processata
int isDuplicate(const char password[MAX_LENGTH], char processedPasswords[][MAX_LENGTH], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(password, processedPasswords[i]) == 0) {
            return 1; // Trovato duplicato
        }
    }
    return 0; // Nessun duplicato trovato
}

// Funzione per tentare di trovare la password
void tryPassword(const char *password, int process_id, int attempt_number) {
    printf("Tentativo %d da processo %d: %s\n", attempt_number, process_id, password);

    if (strcmp(password, "Password00") == 0) { // Password da cercare
        printf("Password trovata da processo %d: %s\n", process_id, password);
        exit(0); // Termina il processo figlio
    }
}

// Funzione per caricare password da un file
int loadPasswords(FILE *file, char passwords[][MAX_LENGTH], int max_count) {
    int count = 0;
    while (count < max_count && fgets(passwords[count], MAX_LENGTH, file) != NULL) {
        passwords[count][strcspn(passwords[count], "\n")] = 0; // Rimuove il newline
        cleanString(passwords[count]); // Pulisce la stringa
        count++;
    }
    return count;
}

// Processa un chunk di password
void processChunk(const char passwords[][MAX_LENGTH], int chunk_count, int *active_processes, int *attempt_count, char processedPasswords[][MAX_LENGTH], int *processed_count) {
    for (int i = 0; i < chunk_count; i++) {
        // Controlla se la password è un duplicato
        if (isDuplicate(passwords[i], processedPasswords, *processed_count)) {
            printf("Chunk duplicato trovato, saltando...\n");
            continue; // Salta la password se è già stata processata
        }

        // Aggiunge la password alla lista di quelle già processate
        strcpy(processedPasswords[*processed_count], passwords[i]);
        (*processed_count)++;

        // Controlla il numero di processi attivi
        while (*active_processes >= MAX_PROCESSES) {
            pid_t wpid = waitpid(-1, NULL, WNOHANG); // Non blocca
            if (wpid > 0) {
                (*active_processes)--; // Un processo è terminato
            }
        }

        pid_t pid = fork();
        if (pid == 0) { // Processo figlio
            tryPassword(passwords[i], getpid(), *attempt_count + 1);
            exit(0); // Termina il processo figlio
        } else if (pid > 0) { // Processo padre
            (*attempt_count)++;
            (*active_processes)++;
        } else {
            perror("Fork fallito");
        }
    }
}

// Processa un file di password
void processFile(const char *filename, int *active_processes, int *attempt_count, char processedPasswords[][MAX_LENGTH], int *processed_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Impossibile aprire il file");
        return;
    }

    char passwords[CHUNK_SIZE][MAX_LENGTH];
    int chunk_count;

    while ((chunk_count = loadPasswords(file, passwords, CHUNK_SIZE)) > 0) {
        processChunk(passwords, chunk_count, active_processes, attempt_count, processedPasswords, processed_count);

        if (chunk_count < CHUNK_SIZE) {
            break; // Fine del file
        }
    }

    // Attende la terminazione di tutti i processi attivi
    while (*active_processes > 0) {
        wait(NULL);
        (*active_processes)--;
    }

    fclose(file);
}

int main() {
    int min_length = 0, max_length = 0;
    char use_length_hint;

    printf("Conosci indicativamente la lunghezza della password? (Y/N): ");
    scanf(" %c", &use_length_hint);

    int active_processes = 0;
    int attempt_count = 0;
    char processedPasswords[1000][MAX_LENGTH]; // Lista delle password processate
    int processed_count = 0; // Numero di password processate

    if (toupper(use_length_hint) == 'Y') {
        printf("Inserisci la lunghezza minima delle password: ");
        scanf("%d", &min_length);
        printf("Inserisci la lunghezza massima delle password: ");
        scanf("%d", &max_length);

        if (min_length <= 0 || max_length <= 0 || min_length > max_length || max_length > MAX_LENGTH) {
            fprintf(stderr, "Dimensioni non valide.\n");
            return 1;
        }

        for (int length = min_length; length <= max_length; length++) {
            char filename[20];
            sprintf(filename, "libreries/p%d.txt", length);
            processFile(filename, &active_processes, &attempt_count, processedPasswords, &processed_count);
        }
    } else {
        processFile("libreries/p10.txt", &active_processes, &attempt_count, processedPasswords, &processed_count);
    }

    printf("La password non è stata trovata.\n");
    return 0;
}
