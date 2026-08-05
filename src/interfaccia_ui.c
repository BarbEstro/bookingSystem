#include "interfaccia_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void interfaccia_utente_cliente() {
    printf("MENU CLIENTE\n");
    printf("1. Visualizza le mie prenotazioni\n");
    printf("2. Effettua una nuova prenotazione\n");
    printf("3. Cancella una prenotazione\n");
    printf("0. Esci\n");
}

void interfaccia_utente_admin() {
    printf("MENU ADMIN\n");
    printf("1. Visualizza tutte le prenotazioni\n");
    printf("2. Gestisci le prenotazioni\n"); //TODO devo fare un sottomenù
    printf("0. Esci\n");
}

void interfaccia_login() {
    printf("1. Login\n");
    printf("2. Registrazione\n");
    printf("0. Esci\n");
}

op_cliente_t operazioni_login() {
    int scelta;
    op_cliente_t operazione;
    printf("Inserisci l'operazione da eseguire (numero): ");
    scanf("%d", &scelta);
    while (scelta < 0 || scelta > 2) {
        printf("Scelta non valida! Inserisci l'operazione da eseguire (numero): ");
        scanf("%d", &scelta);
    }
    switch (scelta) {
        case 1:
            operazione = OP_CLI_LOGIN;
            break;
        case 2:
            operazione = OP_CLI_REGISTRAZIONE;
            break;
        case 0:
            operazione = OP_ESCI;
            printf("Arrivederci.\n");
            break;
    }
    return operazione;
}

op_cliente_t operazioni_cliente() {
    int scelta;
    op_cliente_t operazione;
    printf("Inserisci l'operazione da eseguire (numero): ");
    scanf("%d", &scelta);
    while (scelta < 0 || scelta > 3) {
        printf("Scelta non valida! Inserisci l'operazione da eseguire (numero): ");
        scanf("%d", &scelta);
    }
    switch (scelta) {
        case 1:
            operazione = OP_CLI_MIE_PRENOTAZ;
            break;
        case 2:
            operazione = OP_CLI_NUOVA_PRENOTAZ;
            break;
        case 3:
            operazione = OP_CLI_CANCELLA_PRENOTAZ;
            break;
        case 0:
            printf("Arrivederci.\n");
            exit(EXIT_SUCCESS);
    }
    return operazione;
}

op_cliente_t operazioni_admin() {
    int scelta;
    op_cliente_t operazione;
    printf("Inserisci l'operazione da eseguire (numero): ");
    scanf("%d", &scelta);
    while (scelta < 0 || scelta > 2) {
        printf("Scelta non valida! Inserisci l'operazione da eseguire (numero): ");
        scanf("%d", &scelta);
    }
    switch (scelta) {
        case 1:
            operazione = OP_ADM_LISTA_TUTTE;
            break;
        case 2:
            operazione = OP_ADM_APPROVA_PRENOTAZ; // Puoi modificare questa parte per gestire la scelta dell'operazione
            break;
        case 0:
            printf("Arrivederci.\n");
            exit(EXIT_SUCCESS);
    }
    return operazione;
}
