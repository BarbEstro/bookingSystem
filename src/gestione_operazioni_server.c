#include "gestione_operazioni_server.h"
#include "gestione_login.h"
#include "mappa_prenotazioni.h"
#include "predicati_prenotazioni.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "debug.h"
#include <time.h>

void operazione_login(int client_sock, richiesta_t richiesta) {
    risposta_header_t header;
    header.operazione = OP_CLI_LOGIN;

    printf("Richiesta di login ricevuta: username=%s, password=%s\n", richiesta.utente.username, richiesta.utente.password); //[cite: 6]
    utente_t* utente = verificaCredenziali(richiesta.utente.username, richiesta.utente.password); //[cite: 6]
    
    if(utente != NULL) {
        header.esito = ESITO_OK; //[cite: 6]
        strcpy(header.messaggio, "Login effettuato con successo"); //[cite: 6]
        header.num_elementi = 1;
        header.payload_size = sizeof(utente_t); 

        // 1. Invio prima l'Header, poi l'Utente
        write(client_sock, &header, sizeof(header));
        write(client_sock, utente, header.payload_size);
        
        free(utente); // Libera la memoria allocata per l'utente[cite: 6]
    } else {
        header.esito = ESITO_KO; //[cite: 6]
        strcpy(header.messaggio, "USERNAME o PASSWORD errati"); //[cite: 6]
        header.num_elementi = 0;
        header.payload_size = 0; // Nessun payload
        
        // 2. Mando solo l'header per notificare l'errore
        write(client_sock, &header, sizeof(header));
    }
}

void operazione_registrazione(int client_sock, richiesta_t richiesta) {
    risposta_header_t header;
    header.operazione = OP_CLI_REGISTRAZIONE;
    header.num_elementi = 0;
    header.payload_size = 0; // La registrazione non invia mai payload!

    printf("Richiesta di registrazione ricevuta: username=%s, password=%s, isAdmin=%d\n", richiesta.utente.username, richiesta.utente.password, richiesta.utente.isAdmin); //[cite: 6]
    utente_t* nuovo_utente = crea_utente(richiesta.utente.username, richiesta.utente.password, richiesta.utente.isAdmin); //[cite: 6]
    
    LOG("Fase di controllo utente"); //[cite: 6]
    if (nuovo_utente != NULL){ //[cite: 6]
        if (registraUtente(nuovo_utente)) { //[cite: 6]
            LOG("Non esiste"); //[cite: 6]
            header.esito = ESITO_OK; //[cite: 6]
            strcpy(header.messaggio, "Registrazione effettuata"); //[cite: 6]
        } else {
            LOG("esiste username"); //[cite: 6]
            header.esito = ESITO_KO; //[cite: 6]
            strcpy(header.messaggio, "Registrazione negata, username esistente"); //[cite: 6]
        }
        free(nuovo_utente); // Libera la memoria allocata per il nuovo utente[cite: 6]
    } else {
        header.esito = ESITO_KO; //[cite: 6]
        strcpy(header.messaggio, "Errore interno server");
        printf("Errore nella creazione dell'utente.\n"); //[cite: 6]
    }
    
    // Manda l'esito
    write(client_sock, &header, sizeof(header));
}

void operazione_invia_catalogo_aule(int client_sock, richiesta_t richiesta, risorsa_aula_t* aule, size_t num_aule) {

    risposta_header_t header;
    header.operazione = richiesta.operazione; // Usa automaticamente l'operazione della richiesta

    if (aule != NULL && num_aule > 0) {
        header.esito = ESITO_OK;
        strcpy(header.messaggio, "Elenco aule per selezione");
        header.num_elementi = (int)num_aule;
        header.payload_size = num_aule * sizeof(risorsa_aula_t); 
    } else {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Nessuna risorsa trovata sul server");
        header.num_elementi = 0;
        header.payload_size = 0;
    }

    // 1. Invio dell'header
    write(client_sock, &header, sizeof(header));

    // 2. Invio del payload con l'array di aule
    if (header.esito == ESITO_OK && header.payload_size > 0) {
        write(client_sock, aule, header.payload_size);
    }
}

void operazione_salva_prenotazione(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni) {
    richiesta_prenotazione_t dati_prenotazione;
    risposta_header_t header;
    header.operazione = OP_CLI_NUOVA_PRENOTAZ;
    header.num_elementi = 0;
    header.payload_size = 0;

    // 1. Legge il payload inviato dal client contenente la scelta dell'utente
    if (read(client_sock, &dati_prenotazione, sizeof(richiesta_prenotazione_t)) <= 0) {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Errore nella ricezione dei dati di prenotazione");
        write(client_sock, &header, sizeof(header));
        return;
    }

    // 2. Valida la data e l'orario della prenotazione
    prenotazione_t nuova_p;
    nuova_p.id_risorsa = dati_prenotazione.id_risorsa;
    if(valida_data_e_ora(dati_prenotazione.data, dati_prenotazione.ora_inizio, dati_prenotazione.ora_fine, header.messaggio)) {
        strcpy(nuova_p.data, dati_prenotazione.data);
        strcpy(nuova_p.ora_inizio, dati_prenotazione.ora_inizio);
        strcpy(nuova_p.ora_fine, dati_prenotazione.ora_fine);
        nuova_p.stato = ATTESA;
        nuova_p.utente = richiesta.utente;
        mappa_inserisci_prenotazione(mappa_prenotazioni, dati_prenotazione.id_risorsa, nuova_p);
    } else {
        header.esito = ESITO_KO;
        write(client_sock, &header, sizeof(header));
        return;
    }
    
    header.esito = ESITO_OK;
    strcpy(header.messaggio, "Prenotazione inviata con successo!");

    // 4. Risposta al client
    write(client_sock, &header, sizeof(header));
}

void operazione_lista_mie_prenotazioni(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni) {
    size_t count = 0;
    prenotazione_t* risultati = mappa_filtra_prenotazioni(mappa_prenotazioni, predicato_per_username, richiesta.utente.username, &count);
    invia_elenco_prenotazioni(client_sock, OP_CLI_MIE_PRENOTAZ, risultati, count);
}

// Invia al client l'array 'risultati' (gia' filtrato) come risposta a 'operazione'
static void invia_elenco_prenotazioni(int client_sock, op_cliente_t operazione, prenotazione_t* risultati, size_t count) {
    risposta_header_t header;
    header.operazione = operazione;

    if (risultati != NULL && count > 0) {
        header.esito = ESITO_OK;
        strcpy(header.messaggio, "Elenco prenotazioni");
        header.num_elementi = (int)count;
        header.payload_size = count * sizeof(prenotazione_t);
    } else {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Nessuna prenotazione trovata");
        header.num_elementi = 0;
        header.payload_size = 0;
    }

    write(client_sock, &header, sizeof(header));
    if (header.esito == ESITO_OK) {
        write(client_sock, risultati, header.payload_size);
    }
    free(risultati);
}

// Popola 'buffer' con la data odierna nel formato "YYYY-MM-DD" (es. "2026-09-03")
static void ottieni_data_odierna(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

// Popola 'buffer' con l'ora attuale nel formato "HH:MM" (es. "14:30")
static void ottieni_ora_odierna(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%H:%M", tm_info);
}

static bool valida_data_e_ora(const char *data_req, const char *ora_inizio, const char *ora_fine, char *msg_errore) {
    char data_oggi[11];
    char ora_oggi[6];

    ottieni_data_odierna(data_oggi, sizeof(data_oggi));
    ottieni_ora_odierna(ora_oggi, sizeof(ora_oggi));

    // 1. Controllo coerenza orario: ora_inizio deve precedere ora_fine
    if (strcmp(ora_inizio, ora_fine) >= 0) {
        strcpy(msg_errore, "Errore: L'ora di inizio deve essere precedente all'ora di fine.");
        return false;
    }

    // 2. Controllo Data: non puo essere nel passato
    int cmp_data = strcmp(data_req, data_oggi);
    
    if (cmp_data < 0) {
        strcpy(msg_errore, "Errore: Impossibile prenotare per una data passata.");
        return false;
    }


    // 3. Se la data e OGGI, l'ora di inizio non puo essere gia passata
    if (cmp_data == 0) {
        if (strcmp(ora_inizio, ora_oggi) <= 0) {
            strcpy(msg_errore, "Errore: Per oggi l'ora di inizio deve essere successiva all'ora attuale.");
            return false;
        }
    }

    // 4. Controllo fascia oraria consentita: 08:00 - 19:00
    if (strcmp(ora_inizio, "08:00") < 0 || strcmp(ora_fine, "19:00") > 0) {
      strcpy(msg_errore,
             "Errore: Le prenotazioni sono consentite solo tra le 08:00 e le "
             "19:00.");
      return false;
    }
    return true; // Tutti i controlli sono superati
}

static bool verifica_disponibilita_aula(mappa_t* mappa, int id_aula, const char* data, const char* ora_inizio, const char* ora_fine) {
    nodo_prenotazione_t* lista_prenotazioni = mappa_ottieni_lista(mappa, id_aula);
    if (lista_prenotazioni == NULL) {
        // Nessuna prenotazione esistente per questa aula
        return true;
    }

    while (lista_prenotazioni != NULL) {
        prenotazione_t p = lista_prenotazioni->dato;

        // Controllo se la data coincide
        if (strcmp(p.data, data) == 0) {
            // Controllo sovrapposizione oraria
            if (!(strcmp(ora_fine, p.ora_inizio) <= 0 || strcmp(ora_inizio, p.ora_fine) >= 0)) {
                // Sovrapposizione trovata
                return false;
            }
        }
        lista_prenotazioni = lista_prenotazioni->next;
    }
    return true; // Nessuna sovrapposizione trovata
}