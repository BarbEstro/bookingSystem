#include "interfaccia_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ============================================================================
// 1. STRUTTURA DATI INTERNA
// ============================================================================
typedef struct {
    int tasto;           // Il numero da digitare (es. 1, 2, 0)
    const char *testo;   // La descrizione mostrata a schermo
    op_cliente_t azione; // L'enum associato a quell'opzione
} VoceMenu;

// ============================================================================
// 2. TABELLE DEI MENU (Single Source of Truth)
// Modifica SOLO queste tabelle per aggiungere, rimuovere o rinominare voci!
// ============================================================================

static const VoceMenu MENU_LOGIN[] = {
    {1, "Login", OP_CLI_LOGIN},
    {2, "Registrazione", OP_CLI_REGISTRAZIONE},
    {0, "Esci", OP_ESCI}
};
static const int NUM_LOGIN = sizeof(MENU_LOGIN) / sizeof(MENU_LOGIN[0]);

static const VoceMenu MENU_CLIENTE[] = {
    {1, "Visualizza le mie prenotazioni", OP_CLI_MIE_PRENOTAZ},
    {2, "Effettua una nuova prenotazione", OP_CLI_NUOVA_PRENOTAZ},
    {3, "Cancella una prenotazione", OP_CLI_CANCELLA_PRENOTAZ},
    {0, "Esci", OP_ESCI}
};
static const int NUM_CLIENTE = sizeof(MENU_CLIENTE) / sizeof(MENU_CLIENTE[0]);

static const VoceMenu MENU_ADMIN[] = {
    {1, "Visualizza tutte le prenotazioni", OP_ADM_LISTA_TUTTE},
    {2, "Gestisci le prenotazioni", OP_ADM_APPROVA_PRENOTAZ},
    {0, "Esci", OP_ESCI}
};
static const int NUM_ADMIN = sizeof(MENU_ADMIN) / sizeof(MENU_ADMIN[0]);

// ============================================================================
// 3. FUNZIONI DI SUPPORTO PRIVATE (Generiche per tutti i menu)
// ============================================================================

static void mostra_menu_generico(const char *titolo, const VoceMenu menu[], int size) {
    if (titolo != NULL) {
        printf("%s\n", titolo);
    }
    for (int i = 0; i < size; i++) {
        printf("%d. %s\n", menu[i].tasto, menu[i].testo);
    }
}

static op_cliente_t leggi_scelta_generica(const VoceMenu menu[], int size) {
    int scelta;
    while (1) {
        printf("Inserisci l'operazione da eseguire (numero): ");
        if (scanf("%d", &scelta) != 1) {
            while (getchar() != '\n'); // Svuota il buffer in caso di lettere
            printf("Input non valido! Inserisci un numero.\n");
            continue;
        }

        // Cerca la scelta nella tabella del menu corrente
        for (int i = 0; i < size; i++) {
            if (menu[i].tasto == scelta) {
                if (menu[i].azione == OP_ESCI) {
                    printf("Arrivederci.\n");
                }
                return menu[i].azione;
            }
        }
        printf("Scelta non valida! ");
    }
}

// ============================================================================
// 4. FUNZIONI PUBBLICHE
// Fanno da "ponte": non devi cambiare nulla nel tuo client.c!
// ============================================================================

void interfaccia_login() {
    mostra_menu_generico(NULL, MENU_LOGIN, NUM_LOGIN);
}

op_cliente_t operazioni_login() {
    return leggi_scelta_generica(MENU_LOGIN, NUM_LOGIN);
}

void interfaccia_utente_cliente() {
    mostra_menu_generico("MENU CLIENTE", MENU_CLIENTE, NUM_CLIENTE);
}

op_cliente_t operazioni_cliente() {
    return leggi_scelta_generica(MENU_CLIENTE, NUM_CLIENTE);
}

void interfaccia_utente_admin() {
    mostra_menu_generico("MENU ADMIN", MENU_ADMIN, NUM_ADMIN);
}

op_cliente_t operazioni_admin() {
    return leggi_scelta_generica(MENU_ADMIN, NUM_ADMIN);
}