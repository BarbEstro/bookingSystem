#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "comunicazioneSocket.h"
#include <string.h>
#include "booking_system_struct.h"
#include "interfaccia_ui.h"

void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta);

int main(){
    SocketInfo clientSock = inizializzaSocketClient();
    // Mostra l'interfaccia di login e registrazione
    risposta_server_t risposta;
    bool isAdmin = false;
    do {
        interfaccia_login();
        op_cliente_t scelta = operazioni_login();
        loginOrRegistrazione(clientSock, scelta);
        read(clientSock.socketfd, &risposta, sizeof(risposta));
        switch(risposta.operazione) {
            case OP_SRV_LOGIN_OK:
                printf("Login effettuato con successo! Benvenuto, %s.\n", risposta.utente.username);
                isAdmin = risposta.utente.isAdmin;
                break;
            case OP_SRV_LOGIN_KO:
                printf("Login fallito! Username o password errati.\n");
                break;
            case OP_SRV_REGISTRAZIONE_OK:
                printf("Registrazione effettuata con successo! Puoi ora effettuare il login.\n");
                break;
            case OP_SRV_REGISTRAZIONE_KO:
                printf("Registrazione fallita! L'username potrebbe essere già in uso.\n");
                break;
            default:
                printf("Operazione non riconosciuta dal server.\n");
        }
    } while(risposta.operazione == OP_SRV_LOGIN_KO || risposta.operazione == OP_SRV_REGISTRAZIONE_OK || risposta.operazione == OP_SRV_REGISTRAZIONE_KO);
    
    
    return 0;
}


void loginOrRegistrazione(SocketInfo clientSock, op_cliente_t scelta){
    richiesta_login_registrazione_t richiesta;
    richiesta.operazione = scelta;
    if(scelta != OP_ESCI){
        printf("Inserisci il tuo username [Max 10 caratteri]: ");
        scanf("%s", richiesta.username);
        printf("Inserisci la tua password [Max 20 caratteri]: ");
        scanf("%s", richiesta.password);
    }

    // Invia i dati al server
    write(clientSock.socketfd, &richiesta, sizeof(richiesta));

}