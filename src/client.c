#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "comunicazioneSocket.h"
#include <string.h>
#include "booking_system_struct.h"
#include "interfaccia_ui.h"

void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta);
void manda_richiesta_operazione(SocketInfo clientSock, op_cliente_t scelta, utente_t utente);

int main(){
    SocketInfo clientSock = inizializzaSocketClient();
    utente_t utente_esecuzione;
    // Mostra l'interfaccia di login e registrazione
    risposta_server_t risposta;
    bool isAdmin = false;
    do {
        interfaccia_login();
        op_cliente_t scelta = operazioni_login();
        loginOrRegistrazione(clientSock, scelta);
        read(clientSock.socketfd, &risposta, sizeof(risposta));
        switch(risposta.esito) {
            case ESITO_LOGIN_OK:
                utente_esecuzione = risposta.payload.dati_login.utente;
                printf("Login effettuato con successo! Benvenuto, %s.\n", utente_esecuzione.username);
                isAdmin = utente_esecuzione.isAdmin;
                break;
            case ESITO_LOGIN_KO:
                printf("Login fallito! Username o password errati.\n");
                break;
            case ESITO_REGISTRAZIONE_OK:
                printf("Registrazione effettuata con successo! Puoi ora effettuare il login.\n");
                break;
            case ESITO_REGISTRAZIONE_KO:
                printf("Registrazione fallita! L'username potrebbe essere già in uso.\n");
                break;
            case ESITO_USCITA_OK:
                printf("uscita consentita");
                exit(0);
                break;
            default:
                printf("Operazione non riconosciuta dal server.\n");
        }
    } while(risposta.esito == ESITO_LOGIN_KO || risposta.esito == ESITO_REGISTRAZIONE_KO);
    
    if(isAdmin) {
        printf("Accesso come amministratore.\n");
        interfaccia_utente_admin();
    } else {
        printf("Accesso come cliente.\n");
        interfaccia_utente_cliente();
        op_cliente_t scelta = operazioni_cliente();
        manda_richiesta_operazione(clientSock,scelta,utente_esecuzione);
    }

    
}

//TODO il nome dev'essere tutto minuscolo
//TODO aggiustare gli scanf con %valore
void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta){
    richiesta_t richiesta_login;
    richiesta_login.operazione = scelta;
    if(scelta != OP_ESCI){
        printf("Inserisci il tuo username (tutto minuscolo) [Max 10 caratteri]: ");
        scanf("%s", richiesta_login.utente.username);
        printf("Inserisci la tua password [Max 20 caratteri]: ");
        scanf("%s", richiesta_login.utente.password);
    }

    // Invia i dati al server
    write(clientSock.socketfd, &richiesta_login, sizeof(richiesta_login));

}

void manda_richiesta_operazione(SocketInfo clientSock, op_cliente_t scelta, utente_t utente){
    richiesta_t richiesta;
    richiesta.operazione = scelta;
    richiesta.utente = utente;

    write(clientSock.socketfd, &richiesta, sizeof(richiesta));
}