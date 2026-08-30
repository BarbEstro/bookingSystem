#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "calendario.h"
#include "booking_system_struct.h"
#include "comunicazioneSocket.h"
#include "gestione_operazioni_server.h"
#include "debug.h"

int main() {
  printf("Inizio server...\n");
  SocketInfo serverSock = inizializzaSocketServer();
  struct sockaddr_in clientAddress;
  int currentSocketfd;
  carica_calendario_all_avvio();

  while (1) {
    printf("In attesa di connessioni...\n");
    socklen_t clientAddressLength = sizeof(clientAddress);
    currentSocketfd =
        accept(serverSock.socketfd, (struct sockaddr*)&clientAddress,
               &clientAddressLength);
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
      // Processo figlio
      close(serverSock.socketfd);  // Chiudiamo il socket del server nel processo figlio
      printf("Nuova connessione accettata, creando processo figlio...\n");

      while (1) {
        richiesta_t richiesta;
        read(currentSocketfd, &richiesta, sizeof(richiesta));
        risposta_server_t risposta;
        utente_t utente = richiesta.utente;

        switch (richiesta.operazione) {
          case OP_CLI_LOGIN:
            printf("Gestione operazione di login...\n");
            risposta = operazione_login(richiesta);
            break;
          case OP_CLI_REGISTRAZIONE:
            printf("Gestione operazione di registrazione...\n");
            risposta = operazione_registrazione(richiesta);
            break;
          case OP_ESCI:
            printf("Operazione di uscita richiesta dal client.\n");
            risposta.esito = ESITO_OK;
            break;
          case OP_CLI_MIE_PRENOTAZ:
            printf("Richiesta di visualizzazione prenotazione da: %s", utente.username);
            
            break;
          case OP_CLI_NUOVA_PRENOTAZ:
            printf("Nuova prenotazione richiesta da %s", utente.username);
            //SERVER CHIEDE LA DISPONIBILITÀ AL CALENDARIO CHE DOPO MANDERÀ UNA STRINGA ALL'UTENTE
            //TODO Nuova prenotazione
            break;
          case OP_CLI_CANCELLA_PRENOTAZ:
          //TODO CANCELLAZIONE
            printf("cancellazione..");
          default:
            printf("Operazione non riconosciuta dal server.\n");
        }

        LOG("RISPOSTA DEL SERVER ", risposta.esito);

        write(currentSocketfd, &risposta, sizeof(risposta));

        if (richiesta.operazione == OP_ESCI && risposta.esito == ESITO_OK) {
            printf("Chiusura della connessione con il client...\n");
            close(currentSocketfd);
            exit(EXIT_SUCCESS);
        }
      }
    } else {
      // Processo padre
      printf("Connessione accettata, processo padre continua ad ascoltare...\n");
      close(currentSocketfd);  // Chiudiamo il socket del client nel processo padre
    }
  }
}