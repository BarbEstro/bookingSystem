#include "gestione_operazioni_client.h"
#include "comunicazioneSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void gestisci_operazione_nuova_prenotazione(SocketInfo clientSock, utente_t utente);
static void gestisci_mie_prenotazioni(SocketInfo clientSock, utente_t utente);
static prenotazione_t* richiedi_lista_attesa(SocketInfo clientSock, utente_t utente, int* out_count);
static void stampa_prenotazioni_attesa(const prenotazione_t* prenotazioni, int count);
static void gestisci_lista_attesa(SocketInfo clientSock, utente_t utente);
static void gestisci_gestione_prenotazioni(SocketInfo clientSock, utente_t utente);

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

void gestisci_operazione_admin(SocketInfo clientSock, op_cliente_t scelta, utente_t utente) {
    switch (scelta) {
        case OP_ADM_LISTA_ATTESA:
            gestisci_lista_attesa(clientSock, utente);
            break;

        case OP_ADM_APPROVA_PRENOTAZ:
            gestisci_gestione_prenotazioni(clientSock, utente);
            break;

        default:
            break;
    }
}

static void gestisci_operazione_nuova_prenotazione(SocketInfo clientSock, utente_t utente) {
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

    //stampa aule disponibili
    printf("\n=== AULE DISPONIBILI PER LA PRENOTAZIONE (%d) ===\n", header.num_elementi);
    for (int i = 0; i < header.num_elementi; i++) {
        printf("[%d] %s (Capienza: %d posti)\n", aule[i].id_risorsa, aule[i].nome, aule[i].capienza);
    }
    free(aule); // Liberiamo subito la RAM

    richiesta_prenotazione_t dati_p;
    printf("\n--- Dettagli nuova prenotazione ---\n");

    printf("Inserisci ID dell'aula scelta: ");
    while (scanf("%d", &dati_p.id_risorsa) != 1) {
        while (getchar() != '\n'); 
        printf("[ERRORE] Inserisci un ID numerico valido: ");
    }
    
    printf("Inserisci data (YYYY-MM-DD): ");
    scanf("%10s", dati_p.data);
    printf("Inserisci ora inizio (HH:MM): ");
    scanf("%5s", dati_p.ora_inizio);
    printf("Inserisci ora fine (HH:MM): ");
    scanf("%5s", dati_p.ora_fine);

    //Invia richiesta
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

static void gestisci_mie_prenotazioni(SocketInfo clientSock, utente_t utente) {
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


static prenotazione_t* richiedi_lista_attesa(SocketInfo clientSock, utente_t utente, int* out_count) {
    *out_count = 0;

    richiesta_t req_lista_attesa;
    req_lista_attesa.operazione = OP_ADM_LISTA_ATTESA;
    req_lista_attesa.utente = utente;

    write(clientSock.socketfd, &req_lista_attesa, sizeof(richiesta_t));

    risposta_header_t header;
    if (read(clientSock.socketfd, &header, sizeof(risposta_header_t)) <= 0 || header.esito == ESITO_KO) {
        printf("[INFO] Nessuna prenotazione in attesa.\n");
        return NULL;
    }

    prenotazione_t *lista_attesa = NULL;
    if (header.payload_size > 0) {
        lista_attesa = malloc(header.payload_size);
        if (read(clientSock.socketfd, lista_attesa, header.payload_size) <= 0) {
            printf("[ERRORE] Errore nel trasferimento della lista d'attesa.\n");
            free(lista_attesa);
            return NULL;
        }
    }

    *out_count = header.num_elementi;
    return lista_attesa;
}

static void stampa_prenotazioni_attesa(const prenotazione_t* prenotazioni, int count) {
    printf("\n=== LISTA D'ATTESA (%d) ===\n", count);
    for (int i = 0; i < count; i++) {
        printf("[id %d] id_Aula: %d | Utente: %s | Data: %s | Ora inizio: %s | Ora fine: %s | Stato: %s\n",
               prenotazioni[i].id_prenotazione,
               prenotazioni[i].id_risorsa,
               prenotazioni[i].utente.username,
               prenotazioni[i].data,
               prenotazioni[i].ora_inizio,
               prenotazioni[i].ora_fine,
               stato_a_stringa(prenotazioni[i].stato));
    }
}

static void gestisci_lista_attesa(SocketInfo clientSock, utente_t utente) {
    int count = 0;
    prenotazione_t* lista_attesa = richiedi_lista_attesa(clientSock, utente, &count);
    if (lista_attesa == NULL) return;

    stampa_prenotazioni_attesa(lista_attesa, count);
    free(lista_attesa);
}

static void gestisci_gestione_prenotazioni(SocketInfo clientSock, utente_t utente) {
    int count = 0;
    prenotazione_t* lista_attesa = richiedi_lista_attesa(clientSock, utente, &count);
    if (lista_attesa == NULL || count == 0) {
        free(lista_attesa);
        return;
    }
    stampa_prenotazioni_attesa(lista_attesa, count);

    int id_scelto;
    printf("\nInserisci l'id della prenotazione da gestire (0 per annullare): ");
    if (scanf("%d", &id_scelto) != 1) {
        while (getchar() != '\n'); // Svuota il buffer in caso di lettere
        printf("Input non valido, operazione annullata.\n");
        free(lista_attesa);
        return;
    }
    if (id_scelto == 0) {
        free(lista_attesa);
        return;
    }

    int id_risorsa_scelto = -1;
    for (int i = 0; i < count; i++) {
        if (lista_attesa[i].id_prenotazione == id_scelto) {
            id_risorsa_scelto = lista_attesa[i].id_risorsa;
            break;
        }
    }
    free(lista_attesa);

    if (id_risorsa_scelto < 0) {
        printf("[ERRORE] Id non presente nell'elenco.\n");
        return;
    }

   int scelta_dec;
    richiesta_t req_decisione;
    req_decisione.utente = utente;

    while (1) {
        printf("\nSeleziona l'azione da eseguire per la prenotazione %d:\n", id_scelto);
        printf("1. Accetta\n");
        printf("2. Rifiuta\n");
        printf("0. Annulla e torna al menu\n");
        printf("Scelta: ");

        if (scanf("%d", &scelta_dec) != 1) {
            while (getchar() != '\n'); // Pulisce il buffer in caso di caratteri non numerici
            printf("[ERRORE] Inserisci un numero valido.\n");
            continue;
        }

        if (scelta_dec == 1) {
            req_decisione.operazione = OP_ADM_APPROVA_PRENOTAZ;
            break;
        } else if (scelta_dec == 2) {
            req_decisione.operazione = OP_ADM_RIFIUTA_PRENOTAZ;
            break;
        } else if (scelta_dec == 0) {
            printf("Operazione annullata.\n");
            return;
        } else {
            printf("[ERRORE] Opzione non valida, riprova.\n");
        }
    }

    richiesta_gestione_prenotazione_t dati_decisione;
    dati_decisione.id_prenotazione = id_scelto;
    dati_decisione.id_risorsa = id_risorsa_scelto;

    write(clientSock.socketfd, &req_decisione, sizeof(richiesta_t));
    write(clientSock.socketfd, &dati_decisione, sizeof(richiesta_gestione_prenotazione_t));

    risposta_header_t header_esito;
    if (read(clientSock.socketfd, &header_esito, sizeof(risposta_header_t)) > 0) {
        if (header_esito.esito == ESITO_OK) {
            printf("\n[ESITO OK] %s\n", header_esito.messaggio);
        } else {
            printf("\n[ESITO ERRORE] %s\n", header_esito.messaggio);
        }
    }
}

