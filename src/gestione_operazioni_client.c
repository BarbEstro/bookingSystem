#include "gestione_operazioni_client.h"
#include "comunicazioneSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void gestisci_operazione_cliente(SocketInfo clientSock, op_cliente_t scelta, utente_t utente) {
    switch (scelta) {
        case OP_CLI_NUOVA_PRENOTAZ:
            

            break;

        case OP_CLI_MIE_PRENOTAZ:
            gestisci_mie_prenotazioni(clientSock, utente);
            break;

        case OP_CLI_CANCELLA_PRENOTAZ:
            // TODO
            break;

        default:
            break;
    }
}

void gestisci_operazione_catalogo_aule(SocketInfo clientSock, utente_t utente){
    risposta_header_t header;
    if (read(clientSock.socketfd, &header, sizeof(risposta_header_t)) <= 0) {
        printf("[ERRORE] Impossibile leggere l'header dal server.\n");
        return;
    }

    if (header.esito == ESITO_KO) {
        printf("[ERRORE SERVER] %s\n", header.messaggio);
        return;
    }

    risorsa_aula_t *aule = NULL;

    if (header.payload_size > 0) {
        // Allochiamo in RAM la memoria esatta comunicata dall'header
        aule = malloc(header.payload_size);
        if (aule == NULL) {
            perror("Errore di allocazione memoria per le aule");
            return;
        }

        // Eseguiamo la seconda read leggendo ESATTAMENTE payload_size byte
        if (read(clientSock.socketfd, aule, header.payload_size) <= 0) {
            printf("[ERRORE] Impossibile leggere il catalogo delle aule.\n");
            free(aule);
            return;
        }
    }

    printf("\n=== AULE DISPONIBILI NEL SISTEMA (%d) ===\n", header.num_elementi);
    for (int i = 0; i < header.num_elementi; i++) {
        printf("[%d] %s (Capienza: %d posti)\n", 
               aule[i].id_risorsa, 
               aule[i].nome, 
               aule[i].capienza);
    }

    

    // Una volta stampate, liberiamo la memoria allocata dalla malloc
    free(aule);
}


void gestisci_mie_prenotazioni(SocketInfo clientSock, utente_t utente) {
    // Stessa struttura pulita per visualizzare le prenotazioni dell'utente
}