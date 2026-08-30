#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "booking_system_struct.h"
#include "comunicazioneSocket.h"
#include "interfaccia_ui.h"

void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta);
void manda_richiesta_operazione(SocketInfo clientSock, op_cliente_t scelta, utente_t utente);

int main() {
  SocketInfo clientSock = inizializzaSocketClient();
  utente_t utente_esecuzione;
  // Mostra l'interfaccia di login e registrazione
  risposta_server_t risposta;
  bool isAdmin = false;
  do {
    interfaccia_login();
    op_cliente_t scelta = operazioni_login();
    loginOrRegistrazione(clientSock, scelta);
    read(clientSock.socketfd, &risposta, sizeof(risposta));
    switch (risposta.esito) {
      case ESITO_OK:
        if (scelta == OP_ESCI) {
          printf("%s\n", risposta.messaggio);
          exit(0);
        }

        if (scelta == OP_CLI_LOGIN) {
          utente_esecuzione = risposta.payload.dati_login.utente;
          printf("%s - Benvenuto %s!\n", risposta.messaggio,
                 utente_esecuzione.username);
          isAdmin = utente_esecuzione.isAdmin;
        } else if (scelta == OP_CLI_REGISTRAZIONE) {
          printf("%s\n", risposta.messaggio);
        }
        break;

      case ESITO_KO:
        printf("[ERRORE] %s\n", risposta.messaggio);
        break;

      default:
        printf("Risposta dal server non riconosciuta.\n");
        break;
    }
  } while (risposta.esito == ESITO_KO);

  if (isAdmin) {
    printf("Accesso come amministratore.\n");
    interfaccia_utente_admin();
  } else {
    printf("Accesso come cliente.\n");
    interfaccia_utente_cliente();
    op_cliente_t scelta = operazioni_cliente();
    manda_richiesta_operazione(clientSock, scelta, utente_esecuzione);
  }
}

// TODO il nome dev'essere tutto minuscolo
// TODO aggiustare gli scanf con %valore
void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta) {
  richiesta_t richiesta_login;
  richiesta_login.operazione = scelta;
  if (scelta != OP_ESCI) {
    printf("Inserisci il tuo username (tutto minuscolo) [Max 10 caratteri]: ");
    scanf("%s", richiesta_login.utente.username);
    printf("Inserisci la tua password [Max 20 caratteri]: ");
    scanf("%s", richiesta_login.utente.password);
  }

  // Invia i dati al server
  write(clientSock.socketfd, &richiesta_login, sizeof(richiesta_login));
}

void manda_richiesta_operazione(SocketInfo clientSock, op_cliente_t scelta,utente_t utente) {
  richiesta_t richiesta;
  richiesta.operazione = scelta;
  richiesta.utente = utente;

  write(clientSock.socketfd, &richiesta, sizeof(richiesta));
}