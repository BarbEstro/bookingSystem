#include "interfaccia.h"
#include <stdio.h>
#include <stdbool.h>

static void interfaccia_utente_cliente(char *username) {
    printf("MENU CLIENTE -BENVENUTO %s\n", username);
    printf("1. Visualizza le mie prenotazioni\n");
    printf("2. Effettua una nuova prenotazione\n");
}

static void interfaccia_utente_admin(char *username) {
    printf("MENU ADMIN -BENVENUTO %s\n", username);
    printf("1. Visualizza tutte le prenotazioni\n");
    printf("2. Gestisci le prenotazioni\n");
}

void interfaccia_utente(utente_t *utente) {
    bool isAdmin = utente->isAdmin;
    if (isAdmin) {
        interfaccia_utente_admin(utente->username);
    } else {
        interfaccia_utente_cliente(utente->username);
    }
}  