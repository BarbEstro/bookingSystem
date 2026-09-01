#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "booking_system_struct.h"
#include "comunicazioneSocket.h"
#include "gestione_operazioni_server.h"
#include "debug.h"

risorsa_aula_t* carica_risorse_da_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    size_t num_risorse = file_size / sizeof(risorsa_aula_t);
    
    // Allochiamo spazio per gli elementi + 1 elemento "sentinella" di fine array
    risorsa_aula_t* risorse = malloc((num_risorse + 1) * sizeof(risorsa_aula_t));
    if (!risorse) {
        fclose(file);
        return NULL;
    }

    fread(risorse, sizeof(risorsa_aula_t), num_risorse, file);
    fclose(file);

    // Impostiamo l'ultimo elemento come terminatore
    risorse[num_risorse].id_risorsa = -1; 

    return risorse;
}

int main() {
  printf("Inizio server...\n"); 
  SocketInfo serverSock = inizializzaSocketServer(); 
  struct sockaddr_in clientAddress; 
  int currentSocketfd;
  risorsa_aula_t* risorse = carica_risorse_da_file("dati/risorse.bn");

  while (1) { 
    printf("In attesa di connessioni...\n"); 
    socklen_t clientAddressLength = sizeof(clientAddress); 
    currentSocketfd = accept(serverSock.socketfd, (struct sockaddr*)&clientAddress, &clientAddressLength); 
    if (currentSocketfd < 0) { 
      perror("Errore nell'accept del socket"); 
      exit(EXIT_FAILURE); 
    }

    pid_t pid = fork(); 
    if (pid < 0) { 
      perror("Errore nella creazione del processo figlio"); 
      exit(EXIT_FAILURE); 
    }
    if (pid == 0) {
      // Processo figlio[cite: 9]
      close(serverSock.socketfd);  // Chiudiamo il socket del server nel processo figlio[cite: 9]
      printf("Nuova connessione accettata, creando processo figlio...\n"); 

      while (1) {
        richiesta_t richiesta;
        if (read(currentSocketfd, &richiesta, sizeof(richiesta)) <= 0) {
            printf("Client disconnesso.\n");
            close(currentSocketfd);
            exit(EXIT_FAILURE);
        }

        utente_t utente = richiesta.utente; 

        switch (richiesta.operazione) { 
          case OP_CLI_LOGIN: 
            printf("Gestione operazione di login...\n"); 
            operazione_login(currentSocketfd, richiesta);
            break;

          case OP_CLI_REGISTRAZIONE: 
            printf("Gestione operazione di registrazione...\n"); 
            operazione_registrazione(currentSocketfd, richiesta);
            break;

          case OP_CLI_LISTA_RISORSE: 
            printf("Richiesta di elenco risorse da: %s\n", utente.username); 
            operazione_invia_catalogo_aule(currentSocketfd, richiesta);
            break;

          case OP_CLI_NUOVA_PRENOTAZ: 
            printf("Nuova prenotazione richiesta da %s\n", utente.username);
            operazione_invia_catalogo_aule(currentSocketfd, richiesta);

            break;

          case OP_ESCI: { 
            printf("Operazione di uscita richiesta dal client.\n"); 
            risposta_header_t risposta_esci = { .esito = ESITO_OK, .operazione = OP_ESCI, .payload_size = 0 };
            strcpy(risposta_esci.messaggio, "Disconnessione confermata");
            write(currentSocketfd, &risposta_esci, sizeof(risposta_esci));
            
            printf("Chiusura della connessione con il client...\n"); 
            close(currentSocketfd); 
            exit(EXIT_SUCCESS); 
          }

          case OP_CLI_MIE_PRENOTAZ: 
            printf("Richiesta di visualizzazione prenotazione da: %s\n", utente.username); 
            // TODO
            break;

          case OP_CLI_CANCELLA_PRENOTAZ: 
            printf("cancellazione..\n"); 
            // TODO
            break;

          default: 
            printf("Operazione non riconosciuta dal server.\n"); 
            break;
        }
      }
    } else {
      // Processo padre[cite: 9]
      printf("Connessione accettata, processo padre continua ad ascoltare...\n"); 
      close(currentSocketfd);  // Chiudiamo il socket del client nel processo padre[cite: 9]
    }
  }
}