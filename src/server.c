#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "calendario.h"
#include "booking_system_struct.h"
#include "comunicazioneSocket.h"
#include "gestione_operazioni_server.h"
#include "debug.h"

int main() {
  printf("Inizio server...\n"); //[cite: 9]
  SocketInfo serverSock = inizializzaSocketServer(); //[cite: 9]
  struct sockaddr_in clientAddress; //[cite: 9]
  int currentSocketfd; //[cite: 9]
  carica_calendario_all_avvio(); //[cite: 9]

  while (1) { //[cite: 9]
    printf("In attesa di connessioni...\n"); //[cite: 9]
    socklen_t clientAddressLength = sizeof(clientAddress); //[cite: 9]
    currentSocketfd = accept(serverSock.socketfd, (struct sockaddr*)&clientAddress, &clientAddressLength); //[cite: 9]
    if (currentSocketfd < 0) { //[cite: 9]
      perror("Errore nell'accept del socket"); //[cite: 9]
      exit(EXIT_FAILURE); //[cite: 9]
    }

    pid_t pid = fork(); //[cite: 9]
    if (pid < 0) { //[cite: 9]
      perror("Errore nella creazione del processo figlio"); //[cite: 9]
      exit(EXIT_FAILURE); //[cite: 9]
    }
    if (pid == 0) {
      // Processo figlio[cite: 9]
      close(serverSock.socketfd);  // Chiudiamo il socket del server nel processo figlio[cite: 9]
      printf("Nuova connessione accettata, creando processo figlio...\n"); //[cite: 9]

      while (1) {
        richiesta_t richiesta;
        if (read(currentSocketfd, &richiesta, sizeof(richiesta)) <= 0) {
            printf("Client disconnesso.\n");
            close(currentSocketfd);
            exit(EXIT_FAILURE);
        }

        utente_t utente = richiesta.utente; //[cite: 9]

        switch (richiesta.operazione) { //[cite: 9]
          case OP_CLI_LOGIN: //[cite: 9]
            printf("Gestione operazione di login...\n"); //[cite: 9]
            operazione_login(currentSocketfd, richiesta);
            break;

          case OP_CLI_REGISTRAZIONE: //[cite: 9]
            printf("Gestione operazione di registrazione...\n"); //[cite: 9]
            operazione_registrazione(currentSocketfd, richiesta);
            break;

          case OP_CLI_NUOVA_PRENOTAZ: //[cite: 9]
            printf("Nuova prenotazione richiesta da %s\n", utente.username); //[cite: 9]
            operazione_nuova_prenotazione(currentSocketfd, richiesta);
            break;

          case OP_ESCI: { //[cite: 9]
            printf("Operazione di uscita richiesta dal client.\n"); //[cite: 9]
            risposta_header_t risposta_esci = { .esito = ESITO_OK, .operazione = OP_ESCI, .payload_size = 0 };
            strcpy(risposta_esci.messaggio, "Disconnessione confermata");
            write(currentSocketfd, &risposta_esci, sizeof(risposta_esci));
            
            printf("Chiusura della connessione con il client...\n"); //[cite: 9]
            close(currentSocketfd); //[cite: 9]
            exit(EXIT_SUCCESS); //[cite: 9]
          }

          case OP_CLI_MIE_PRENOTAZ: //[cite: 9]
            printf("Richiesta di visualizzazione prenotazione da: %s\n", utente.username); //[cite: 9]
            // TODO
            break;

          case OP_CLI_CANCELLA_PRENOTAZ: //[cite: 9]
            printf("cancellazione..\n"); //[cite: 9]
            // TODO
            break;

          default: //[cite: 9]
            printf("Operazione non riconosciuta dal server.\n"); //[cite: 9]
            break;
        }
      }
    } else {
      // Processo padre[cite: 9]
      printf("Connessione accettata, processo padre continua ad ascoltare...\n"); //[cite: 9]
      close(currentSocketfd);  // Chiudiamo il socket del client nel processo padre[cite: 9]
    }
  }
}