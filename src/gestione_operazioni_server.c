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
        risposta.esito = ESITO_LOGIN_OK;
        risposta.payload.dati_login.utente = *utente;
        free(utente); // Libera la memoria allocata per l'utente
    } else {
        risposta.esito = ESITO_LOGIN_KO;
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
            risposta.esito = ESITO_REGISTRAZIONE_OK;
        } else {
            LOG("esiste username");
            risposta.esito = ESITO_REGISTRAZIONE_KO;
        }
        free(nuovo_utente); // Libera la memoria allocata per il nuovo utente
    } else {
        risposta.esito = ESITO_REGISTRAZIONE_KO; // TODO QUESTA LA VEDO COME ANOMALIA
        printf("Errore nella creazione dell'utente.\n");
    }
    LOG("RISPOSTA SERVER: ", risposta);
    return risposta;
}

risposta_server_t operazione_nuova_prenotazione(richiesta_t richiesta){

}

