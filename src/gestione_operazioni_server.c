#include "gestione_operazioni_server.h"
#include "gestione_login.h"
#include "calendario.h" // Per usare i getter
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

void operazione_nuova_prenotazione(int client_sock, richiesta_t richiesta){
    risposta_header_t header;
    header.operazione = OP_CLI_NUOVA_PRENOTAZ;

    // Recuperiamo il calendario caricato in RAM
    int tot_aule = get_num_aule_totali(); 
    disponibilita_aula_t *calendario = get_calendario_in_ram(); 

    header.esito = ESITO_OK;
    strcpy(header.messaggio, "Lista aule disponibili");
    header.num_elementi = tot_aule;
    header.payload_size = tot_aule * sizeof(disponibilita_aula_t);

    // 1. Invio prima l'Header
    write(client_sock, &header, sizeof(header));
    // 2. Se ci sono aule, invio l'intero array direttamente dalla memoria
    if (header.payload_size > 0) {
        write(client_sock, calendario, header.payload_size);
    }
}