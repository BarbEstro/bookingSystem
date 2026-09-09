#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "booking_system_struct.h"
#include "comunicazioneSocket.h"
#include "gestione_operazioni_server.h"
#include "debug.h"
#include "mappa_prenotazioni.h"

static risorsa_aula_t* carica_risorse_da_file(const char* filename, size_t* out_num_risorse) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    size_t num_risorse = file_size / sizeof(risorsa_aula_t);

    // Allocazione dinamica + 1 elemento sentinella
    risorsa_aula_t* risorse = malloc((num_risorse + 1) * sizeof(risorsa_aula_t));
    if (!risorse) {
        fclose(file);
        return NULL;
    }

    fread(risorse, sizeof(risorsa_aula_t), num_risorse, file);
    fclose(file);

    // Sentinella di fine array per iterazioni veloci
    risorse[num_risorse].id_risorsa = -1; 

    if (out_num_risorse != NULL) {
        *out_num_risorse = num_risorse;
    }

    return risorse;
}

static bool init_ram(const char* filename_risorse, 
                        risorsa_aula_t** out_aule, 
                        size_t* out_num_risorse, 
                        mappa_t** out_mappa) {
    size_t num_aule = 0;

    // 1. Carica le aule dal file in RAM
    *out_aule = carica_risorse_da_file(filename_risorse, &num_aule);
    if (*out_aule == NULL) {
        perror("Errore durante il caricamento del file risorse");
        return false;
    }

    // 2. Crea la mappa con il numero esatto (e dinamico) di aule caricate
    *out_mappa = crea_mappa(*out_aule, num_aule);
    if (*out_mappa == NULL) {
        fprintf(stderr, "Errore nella allocazione della mappa prenotazioni.\n");
        free(*out_aule);
        *out_aule = NULL;
        return false;
    }

    if (out_num_risorse != NULL) {
        *out_num_risorse = num_aule;
    }

    return true;
}



int main() {
  printf("Inizio server...\n"); 
  SocketInfo serverSock = inizializzaSocketServer();
  struct sockaddr_in clientAddress; 
  int currentSocketfd;
  size_t num_aule = 0;
  mappa_t* mappa_prenotazioni = NULL;
  risorsa_aula_t* out_aule = NULL;
  if(init_ram("dati/risorse.bn", &out_aule, &num_aule, &mappa_prenotazioni) == false) {
      fprintf(stderr, "Errore nell'inizializzazione della RAM.\n");
      exit(EXIT_FAILURE);
  }

  signal(SIGCHLD, SIG_IGN);
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
      // Processo figlio
      close(serverSock.socketfd);  // Chiudiamo il socket del server nel processo figlio
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
            operazione_invia_catalogo_aule(currentSocketfd, richiesta, out_aule, num_aule);
            break;

          case OP_CLI_NUOVA_PRENOTAZ: 
            printf("Nuova prenotazione richiesta da %s\n", utente.username);
            operazione_salva_prenotazione(currentSocketfd, richiesta, mappa_prenotazioni);
            break;

          case OP_CLI_MIE_PRENOTAZ: 
            printf("Richiesta di visualizzazione prenotazione da: %s\n", utente.username); 
            operazione_lista_mie_prenotazioni(currentSocketfd, richiesta, mappa_prenotazioni);
            break;

          case OP_ADM_LISTA_ATTESA:
            printf("Richiesta elenco di tutte le prenotazioni da: %s\n", utente.username);
            operazione_lista_attesa_prenotazioni(currentSocketfd, richiesta, mappa_prenotazioni);
            break;

          case OP_ADM_APPROVA_PRENOTAZ:
          case OP_ADM_RIFIUTA_PRENOTAZ:
            printf("Gestione approvazione/rifiuto prenotazione da: %s\n", utente.username);
            operazione_gestisci_prenotazione(currentSocketfd, richiesta, mappa_prenotazioni);
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

          default: 
            printf("Operazione non riconosciuta dal server.\n"); 
            break;
        }
      }
    } else {
      // Processo padre
      printf("Connessione accettata, processo padre continua ad ascoltare...\n"); 
      close(currentSocketfd);  // Chiudiamo il socket del client nel processo padre
    }
  }
}