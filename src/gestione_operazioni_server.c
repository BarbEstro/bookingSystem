#include "gestione_operazioni_server.h"
#include "gestione_login.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "debug.h"

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

void operazione_invia_catalogo_aule(int client_sock, richiesta_t richiesta) {
    size_t num_aule = 0;
    
    // 1. Carica le aule dal file binario "dati/risorse.bn"
    // La funzione calcola automaticamente quante aule ci sono nel file!
    risorsa_aula_t* aule = carica_risorse_da_file("dati/risorse.bn", &num_aule);

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

    // 2. Invio dell'header[cite: 5]
    write(client_sock, &header, sizeof(header));

    // 3. Invio dell'array automatico[cite: 5]
    if (header.payload_size > 0) {
        write(client_sock, aule, header.payload_size);
    }

    // 4. Libera la memoria allocata dalla malloc della carica_risorse_da_file
    free(aule);
}

void operazione_salva_prenotazione(int client_sock, richiesta_t richiesta) {
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

    // 2. Prepara la struct prenotazione completa
    prenotazione_t nuova_p;
    nuova_p.id_risorsa = dati_prenotazione.id_risorsa;
    strcpy(nuova_p.data, dati_prenotazione.data);
    strcpy(nuova_p.ora_inizio, dati_prenotazione.ora_inizio);
    strcpy(nuova_p.ora_fine, dati_prenotazione.ora_fine);
    nuova_p.utente = richiesta.utente;
    nuova_p.stato = ATTESA; // La prenotazione nasce in attesa di approvazione/conferma

    // 3. Controllo conflitti orari ed eventuale salvataggio (da implementare con file/RAM)
    // if (verifica_disponibilita_aula(...)) { ... }
    
    header.esito = ESITO_OK;
    strcpy(header.messaggio, "Prenotazione inviata con successo!");

    // 4. Risposta al client
    write(client_sock, &header, sizeof(header));
}

