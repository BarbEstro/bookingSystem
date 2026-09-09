#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "booking_system_struct.h"
#include "comunicazioneSocket.h"
#include "gestione_operazioni_client.h"
#include "interfaccia_ui.h"

void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta);
void manda_richiesta_operazione(SocketInfo clientSock, op_cliente_t scelta,utente_t utente);
void str_tolower(char *str);

int main() {
  SocketInfo clientSock = inizializzaSocketClient();
  utente_t utente_esecuzione;
  risposta_header_t risposta;
  bool isAdmin = false;

  do {
    interfaccia_login();
    op_cliente_t scelta = operazioni_login();
    loginOrRegistrazione(clientSock, scelta);

    // 1. PRIMA READ: Leggiamo solo l'Header (le "istruzioni")
    if (read(clientSock.socketfd, &risposta, sizeof(risposta)) <= 0) {
      printf("Errore di comunicazione con il server.\n");
      exit(1);
    }

    switch (risposta.esito) {
      case ESITO_OK:
        if (scelta == OP_ESCI) {
          printf("%s\n", risposta.messaggio);
          exit(0);
        }

        if (scelta == OP_CLI_LOGIN) {
          // 2. SECONDA READ: Il server ha detto OK e ci sta mandando l'utente
          if (risposta.payload_size > 0) {
            read(clientSock.socketfd, &utente_esecuzione,
                 risposta.payload_size);
          }

          printf("%s - Benvenuto %s!\n", risposta.messaggio,
                 utente_esecuzione.username);
          isAdmin = utente_esecuzione.isAdmin;

        } else if (scelta == OP_CLI_REGISTRAZIONE) {
          // Nessuna seconda read: la registrazione non invia payload
          // (payload_size = 0)
          printf("%s\n", risposta.messaggio);
        }
        break;

      case ESITO_KO:
        // In caso di errore (es. password errata), il server non invia payload.
        // Leggiamo e stampiamo solo il messaggio dell'header.
        printf("[ERRORE] %s\n", risposta.messaggio);
        break;

      default:
        printf("Risposta dal server non riconosciuta.\n");
        break;
    }
  } while (risposta.esito == ESITO_KO);

  op_cliente_t scelta_utente;

  if (isAdmin) {
    do {
      printf("Accesso come amministratore.\n");
      interfaccia_utente_admin();
      scelta_utente = operazioni_admin();
      gestisci_operazione_admin(clientSock, scelta_utente, utente_esecuzione);

    } while (scelta_utente != OP_ESCI);
  } else {
    do {
      printf("Accesso come cliente.\n");
      interfaccia_utente_cliente();
      scelta_utente = operazioni_cliente();
      gestisci_operazione_cliente(clientSock, scelta_utente, utente_esecuzione);

    } while (scelta_utente != OP_ESCI);
  }
}

// =======================================================
// Le due funzioni sottostanti rimangono invariate
// =======================================================


void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta) {
  richiesta_t richiesta;
  richiesta.operazione = scelta;
  if (scelta != OP_ESCI) {
    printf("Inserisci il tuo username (tutto minuscolo) [Max 10 caratteri]: ");
    scanf("%10s", richiesta.utente.username);
    str_tolower(richiesta.utente.username);
    printf("Inserisci la tua password [Max 20 caratteri]: ");
    scanf("%20s", richiesta.utente.password);
  }

  // Invia i dati al server
  write(clientSock.socketfd, &richiesta, sizeof(richiesta));
}

void str_tolower(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower((unsigned char)str[i]);
  }
}