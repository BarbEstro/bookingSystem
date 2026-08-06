#include "gestione_operazioni_server.h"
#include "gestione_login.h"
#include <stdio.h>
#include <stdlib.h>

risposta_server_t operazione_login(richiesta_login_registrazione_t richiesta) {
    risposta_server_t risposta;
    printf("Richiesta di login ricevuta: username=%s, password=%s\n", richiesta.username, richiesta.password);
    utente_t* utente = verificaCredenziali(richiesta.username, richiesta.password);
    if(utente != NULL) {
        risposta.operazione = OP_SRV_LOGIN_OK;
        risposta.utente = *utente;
        free(utente); // Libera la memoria allocata per l'utente
    } else {
        risposta.operazione = OP_SRV_LOGIN_KO;
    }

    return risposta;
}

risposta_server_t operazione_registrazione(richiesta_login_registrazione_t richiesta) {
    risposta_server_t risposta;
    printf("Richiesta di registrazione ricevuta: username=%s, password=%s, isAdmin=%d\n", richiesta.username, richiesta.password, richiesta.isAdmin);
    utente_t* nuovo_utente = crea_utente(richiesta.username, richiesta.password, richiesta.isAdmin);
    if (nuovo_utente != NULL){
        if (registraUtente(nuovo_utente)) {
            risposta.operazione = OP_SRV_REGISTRAZIONE_OK;
        } else {
            risposta.operazione = OP_SRV_REGISTRAZIONE_KO;
        }
        free(nuovo_utente); // Libera la memoria allocata per il nuovo utente
    } else {
        risposta.operazione = OP_SRV_REGISTRAZIONE_KO; // TODO QUESTA LA VEDO COME ANOMALIA
        printf("Errore nella creazione dell'utente.\n");
    }
    return risposta;
}