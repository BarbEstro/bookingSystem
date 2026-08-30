#include "gestione_operazioni_server.h"
#include "gestione_login.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

risposta_server_t operazione_login(richiesta_t richiesta) {
    risposta_server_t risposta;
    utente_t utente_temp = richiesta.utente;
    printf("Richiesta di login ricevuta: username=%s, password=%s\n", utente_temp.username, utente_temp.password);
    utente_t* utente = verificaCredenziali(utente_temp.username, utente_temp.password);
    if(utente != NULL) {
        risposta.esito = ESITO_OK;
        risposta.payload.dati_login.utente = *utente;
        strcpy(risposta.messaggio, "Login effettuato con successo");
        free(utente); // Libera la memoria allocata per l'utente
    } else {
        risposta.esito = ESITO_KO;
        strcpy(risposta.messaggio, "USERNAME o PASSWORD errati");
    }

    return risposta;
}

risposta_server_t operazione_registrazione(richiesta_t richiesta) {
    risposta_server_t risposta;
    utente_t utente_temp = richiesta.utente;
    printf("Richiesta di registrazione ricevuta: username=%s, password=%s, isAdmin=%d\n", utente_temp.username, utente_temp.password, utente_temp.isAdmin);
    utente_t* nuovo_utente = crea_utente(utente_temp.username, utente_temp.password, utente_temp.isAdmin);
    LOG("Fase di controllo utente");
    if (nuovo_utente != NULL){
        if (registraUtente(nuovo_utente)) {
            LOG("Non esiste");
            risposta.esito = ESITO_OK;
            strcpy(risposta.messaggio, "Registrazione effettuata");
        } else {
            LOG("esiste username");
            strcpy(risposta.messaggio, "Registrazione negata, username esistente");
            risposta.esito = ESITO_KO;
        }
        free(nuovo_utente); // Libera la memoria allocata per il nuovo utente
    } else {
        risposta.esito = ESITO_KO; // TODO QUESTA LA VEDO COME ANOMALIA
        printf("Errore nella creazione dell'utente.\n");
    }
    LOG("RISPOSTA SERVER: ", risposta);
    return risposta;
}

risposta_server_t operazione_nuova_prenotazione(richiesta_t richiesta){

}

