#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_LENGTH 30
#define POPULATION_SIZE 5 // Dimensione della popolazione per il "guessing" intelligente
#define MAX_PASSWORDS 1000 // Numero massimo di password da gestire

// Funzione per pulire il terminale
void clearTerminal() {
    system("clear");
}

void tryPassword(const char *password, int attempt_number, int *found) {
    if (*found) return; // Se la password è già stata trovata, esci dalla funzione

    printf("Tentativo %d: %s\n", attempt_number, password);

    // Simula il tentativo
    if (strcmp(password, "Password00") == 0) { // Cambia "Password00" con la tua password da trovare
        printf("Password trovata: %s\n", password);
        *found = 1; // Imposta flag per indicare che la password è stata trovata
    }
}

// Funzione per generare permutazioni
void permute(char *str, int l, int r, const char *original_password, int *perm_count, int *found) {
    if (*found) return; // Se la password è già stata trovata, esci dalla funzione

    if (l == r) {
        clearTerminal();
        (*perm_count)++;
        printf("** uso permutazioni **\n");
        printf("parola originale -> %s\n", original_password);
        printf("permutazione n° -> %d\n", *perm_count);
        printf("parola permutata -> %s\n", str);

        // Prova la permutazione
        tryPassword(str, *perm_count, found);
    } else {
        for (int i = l; i <= r; i++) {
            // Scambia
            char temp = str[l];
            str[l] = str[i];
            str[i] = temp;

            // Permutazioni ricorsive
            permute(str, l + 1, r, original_password, perm_count, found);

            // Ripristina
            temp = str[l];
            str[l] = str[i];
            str[i] = temp;
        }
    }
}

// Funzione per generare password casuali
void generateRandomPasswords(char passwords[][MAX_LENGTH], int num_passwords) {
    const char *words[] = {"pass", "word", "123", "abc", "test"}; // Array di parole di esempio
    for (int i = 0; i < num_passwords; i++) {
        snprintf(passwords[i], MAX_LENGTH, "%s%d", words[rand() % 5], rand() % 100);
    }
}

// Funzione per caricare password da un file
int loadPasswords(const char *filename, char passwords[][MAX_LENGTH], int max_count) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int count = 0;
    char password[MAX_LENGTH];
    while (fgets(password, sizeof(password), file)) {
        password[strcspn(password, "\n")] = 0; // Rimuovi newline
        if (strlen(password) > 0 && count < max_count) {
            strcpy(passwords[count], password);
            count++;
        }
    }
    fclose(file);
    return count;
}

void processFile(const char *filename, int *attempt_count, int *found, char tested_passwords[MAX_PASSWORDS][MAX_LENGTH], int *tested_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Impossibile aprire il file");
        return;
    }

    char password[MAX_LENGTH];
    int chunk_number = 0; // Per tenere traccia del numero del chunk
    while (fgets(password, sizeof(password), file)) {
        password[strcspn(password, "\n")] = 0; // Rimuovi newline
        chunk_number++; // Incrementa il numero del chunk

        // Ignora righe vuote
        if (strlen(password) == 0) {
            continue;
        }

        // Controlla se la password è già stata testata
        for (int i = 0; i < *tested_count; i++) {
            if (strcmp(password, tested_passwords[i]) == 0) {
                printf("Chunk duplicato trovato al numero %d, ignorando: %s\n", chunk_number, password);
                continue;
            }
        }

        // Ignora password con caratteri non validi
        int valid = 1;
        for (int i = 0; i < strlen(password); i++) {
            if (!isalnum(password[i])) {
                printf("Password ignorata (carattere non valido): %s\n", password);
                valid = 0;
                break;
            }
        }
        if (!valid) continue;

        // Controlla se ci sono spazio sufficiente nel array tested_passwords
        if (*tested_count < MAX_PASSWORDS) {
            // Aggiungi la password alla lista di quelle già testate
            strcpy(tested_passwords[*tested_count], password);
            (*tested_count)++;
        } else {
            printf("Limite massimo di password testate raggiunto.\n");
            break; // Esci se il limite è raggiunto
        }

        // Prova la password
        (*attempt_count)++;
        tryPassword(password, *attempt_count, found);
        
        // Controlla se la password è stata trovata
        if (*found) break; // Esci se la password è stata trovata
    }

    fclose(file);
}

// Funzione per l'attacco ibrido con permutazioni e generazione di password
void attackWithHybridMethods(int min_length, const char *original_password) {
    char passwords[POPULATION_SIZE][MAX_LENGTH];
    generateRandomPasswords(passwords, POPULATION_SIZE); // Genera password casuali

    printf("La password non è stata trovata nei file. Attivazione permutazioni e generazione casuale...\n");

    int perm_count = 0;
    int found = 0; // Flag per indicare se la password è stata trovata
    // Prova tutte le permutazioni
    for (int i = 0; i < POPULATION_SIZE; i++) {
        if (strlen(passwords[i]) > 0) { // Assicurati che la password non sia vuota
            permute(passwords[i], 0, strlen(passwords[i]) - 1, original_password, &perm_count, &found);
        } else {
            fprintf(stderr, "Errore: password generata vuota, saltando...\n");
        }
        if (found) break; // Esci se la password è stata trovata
    }
}

int main() {
    int min_length = 0, max_length = 0;
    char use_length_hint;

    printf("Conosci indicativamente la lunghezza della password? (Y/N): ");
    scanf(" %c", &use_length_hint);

    int attempt_count = 0;
    int found = 0; // Flag per indicare se la password è stata trovata
    char tested_passwords[MAX_PASSWORDS][MAX_LENGTH]; // Array per tenere traccia delle password testate
    int tested_count = 0; // Contatore per le password testate

    if (toupper(use_length_hint) == 'Y') {
        printf("Inserisci la lunghezza minima delle password: ");
        scanf("%d", &min_length);
        printf("Inserisci la lunghezza massima delle password: ");
        scanf("%d", &max_length);

        if (min_length <= 0 || max_length <= 0 || min_length > max_length || max_length > MAX_LENGTH) {
            fprintf(stderr, "Dimensioni non valide.\n");
            return 1;
        }

        // Processa i file da min_length a max_length
        for (int length = min_length; length <= max_length; length++) {
            char filename[20];
            sprintf(filename, "libreries/p%d.txt", length);
            processFile(filename, &attempt_count, &found, tested_passwords, &tested_count);
            if (found) break; // Esci se la password è stata trovata
        }
    } else {
        // Processa tutti i file pN.txt
        for (int length = 1; length < MAX_LENGTH; length++) {
            char filename[20];
            sprintf(filename, "libreries/p%d.txt", length);
            processFile(filename, &attempt_count, &found, tested_passwords, &tested_count);
            if (found) break; // Esci se la password è stata trovata
        }
    }

    // Se la password non è stata trovata, inizia attacco ibrido
    if (!found) {
        printf("La password non è stata trovata, inizio attacco ibrido...\n");
        attackWithHybridMethods(min_length, "Password00"); // Sostituisci con la password da cercare
    }

    return 0;
}
