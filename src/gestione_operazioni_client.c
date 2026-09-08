#include "gestione_operazioni_client.h"
#include "comunicazioneSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Converte lo stato numerico della prenotazione nella sua descrizione testuale
static const char* stato_a_stringa(enum stato_prenotazione stato) {
    switch (stato) {
        case ATTESA: return "In attesa";
        case APPROVATA: return "Approvata";
        case RIFIUTATA: return "Rifiutata";
        default: return "Sconosciuto";
    }
}

void gestisci_operazione_cliente(SocketInfo clientSock, op_cliente_t scelta, utente_t utente) {
    switch (scelta) {
        case OP_CLI_NUOVA_PRENOTAZ:
            gestisci_operazione_nuova_prenotazione(clientSock, utente);
            break;

        case OP_CLI_MIE_PRENOTAZ:
            gestisci_mie_prenotazioni(clientSock, utente);
            break;

        default:
            break;
    }
}

void gestisci_operazione_nuova_prenotazione(SocketInfo clientSock, utente_t utente) {
    // =========================================================================
    // FASE 1: Richiesta catalogo aule al server
    // =========================================================================
    richiesta_t req_aule;
    req_aule.operazione = OP_CLI_LISTA_RISORSE;
    req_aule.utente = utente;
    
    // Invia richiesta d'elenco aule
    write(clientSock.socketfd, &req_aule, sizeof(richiesta_t));

    // Legge la risposta delle aule
    risposta_header_t header;
    if (read(clientSock.socketfd, &header, sizeof(risposta_header_t)) <= 0 || header.esito == ESITO_KO) {
        printf("[ERRORE] Impossibile recuperare il catalogo delle aule.\n");
        return;
    }

    risorsa_aula_t *aule = NULL;
    if (header.payload_size > 0) {
        aule = malloc(header.payload_size);
        if (read(clientSock.socketfd, aule, header.payload_size) <= 0) {
            printf("[ERRORE] Errore nel trasferimento delle aule.\n");
            free(aule);
            return;
        }
    }

    // =========================================================================
    // FASE 2: Stampa aule e raccolta scelta utente
    // =========================================================================
    printf("\n=== AULE DISPONIBILI PER LA PRENOTAZIONE (%d) ===\n", header.num_elementi);
    for (int i = 0; i < header.num_elementi; i++) {
        printf("[%d] %s (Capienza: %d posti)\n", aule[i].id_risorsa, aule[i].nome, aule[i].capienza);
    }
    free(aule); // Liberiamo subito la RAM

    richiesta_prenotazione_t dati_p;
    printf("\n--- Dettagli nuova prenotazione ---\n");
    printf("Inserisci ID dell'aula scelta: ");
    scanf("%d", &dati_p.id_risorsa);
    printf("Inserisci data (YYYY-MM-DD): ");
    scanf("%10s", dati_p.data);
    printf("Inserisci ora inizio (HH:MM): ");
    scanf("%5s", dati_p.ora_inizio);
    printf("Inserisci ora fine (HH:MM): ");
    scanf("%5s", dati_p.ora_fine);

    // =========================================================================
    // FASE 3: Invia la vera e propria richiesta di prenotazione
    // =========================================================================
    richiesta_t req_prenotazione;
    req_prenotazione.operazione = OP_CLI_NUOVA_PRENOTAZ;
    req_prenotazione.utente = utente;

    // Invia prima la richiesta con opzione NUOVA_PRENOTAZ
    write(clientSock.socketfd, &req_prenotazione, sizeof(richiesta_t));
    // Subito dopo invia il payload con le scelte dell'utente
    write(clientSock.socketfd, &dati_p, sizeof(richiesta_prenotazione_t));

    // Legge l'esito finale restituito dal server
    if (read(clientSock.socketfd, &header, sizeof(risposta_header_t)) > 0) {
        if (header.esito == ESITO_OK) {
            printf("\n[ESITO OK] %s\n", header.messaggio);
        } else {
            printf("\n[ESITO ERRORE] %s\n", header.messaggio);
        }
    }
}

void gestisci_mie_prenotazioni(SocketInfo clientSock, utente_t utente) {
    richiesta_t req_mie_prenotazioni;
    req_mie_prenotazioni.operazione = OP_CLI_MIE_PRENOTAZ;
    req_mie_prenotazioni.utente = utente;

    write(clientSock.socketfd, &req_mie_prenotazioni, sizeof(richiesta_t));

    risposta_header_t header;
    if (read(clientSock.socketfd, &header, sizeof(risposta_header_t)) <= 0 || header.esito == ESITO_KO) {
        printf("[ERRORE] Impossibile recuperare le prenotazioni.\n");
        return;
    }

    prenotazione_t *mie_prenotazioni = NULL;
    if (header.payload_size > 0) {
        mie_prenotazioni = malloc(header.payload_size);
        if (read(clientSock.socketfd, mie_prenotazioni, header.payload_size) <= 0) {
            printf("[ERRORE] Errore nel trasferimento delle prenotazioni.\n");
            free(mie_prenotazioni);
            return;
        }
    }

    printf("\n=== LE MIE PRENOTAZIONI (%d) ===\n", header.num_elementi);
    for (int i = 0; i < header.num_elementi; i++) {
        printf("[%d] id_Aula: %d | Data: %s | Ora inizio: %s | Ora fine: %s | Stato: %s\n",
               mie_prenotazioni[i].id_prenotazione,
               mie_prenotazioni[i].id_risorsa, 
               mie_prenotazioni[i].data,
               mie_prenotazioni[i].ora_inizio,
               mie_prenotazioni[i].ora_fine,
               stato_a_stringa(mie_prenotazioni[i].stato));
    }
    free(mie_prenotazioni);
}
