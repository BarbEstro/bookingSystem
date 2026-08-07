#ifndef BOOKING_SYSTEM_STRUCT_H
#define BOOKING_SYSTEM_STRUCT_H

#define MAX_USER_LEN  10
#define MAX_PASS_LEN  20

#include <stdbool.h>
#include <stdio.h>

enum stato_prenotazione {
    ATTESA,
    APPROVATA,
    RIFIUTATA
};

typedef enum {
    OP_CLI_LOGIN,
    OP_CLI_REGISTRAZIONE,
    OP_CLI_LISTA_RISORSE,
    OP_CLI_NUOVA_PRENOTAZ,
    OP_CLI_MIE_PRENOTAZ,
    OP_CLI_CANCELLA_PRENOTAZ,

    OP_ADM_LISTA_TUTTE,
    OP_ADM_APPROVA_PRENOTAZ,
    OP_ADM_RIFIUTA_PRENOTAZ,

    OP_ESCI
} op_cliente_t;

typedef enum {
    ESITO_LOGIN_OK = 1001,
    ESITO_LOGIN_KO,
    ESITO_REGISTRAZIONE_OK,
    ESITO_REGISTRAZIONE_KO,
    ESITO_USCITA_OK
} esito_server_t;

typedef struct {
    int id_risorsa;             // Identificatore unico dell'aula (es. 101)
    char nome[30];              // Es. "Aula Magna"
    int capienza;               // Es. 50
} risorsa_aula_t;

typedef struct {
    char username[MAX_USER_LEN + 1];
    char password[MAX_PASS_LEN + 1];
    bool isAdmin;   
} utente_t;

typedef struct {
    int id_prenotazione;
    int id_risorsa;
    char data[11]; // Formato: YYYY-MM-DD
    char ora_inizio[6]; // Formato: HH:MM
    char ora_fine[6]; // Formato: HH:MM
    utente_t utente;
    enum stato_prenotazione stato;
} prenotazione_t;

typedef struct {
    op_cliente_t operazione;
    utente_t utente;
} richiesta_t;

typedef struct {
    esito_server_t esito;
    utente_t utente;
} risposta_server_t;


#endif // BOOKING_SYSTEM_STRUCT_H