#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Crea il socket del client
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Imposta l'indirizzo del server
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("146.241.145.174"); // Indirizzo IP del server
    server_address.sin_port = htons(PORT);

    // Connetti al server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }

    printf("Connessione al server riuscita.\n");

    // Leggi i comandi da stdin e invia al server
    while (1) {
        printf(">> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Errore nella lettura dell'input");
            break;
        }

        // Rimuovi il newline dal comando
        buffer[strcspn(buffer, "\n")] = '\0';

        // Invia il comando al server
        ssize_t bytes_sent = send(client_socket, buffer, strlen(buffer), 0);
        if (bytes_sent < 0) {
            perror("Errore nell'invio del comando al server");
            break;
        }

        // Ricevi e stampa la risposta dal server
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            perror("Errore nella ricezione della risposta dal server");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }

    // Chiudi il socket
    close(client_socket);

    return 0;
}
