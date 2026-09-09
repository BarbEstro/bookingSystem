#ifndef BOOKING_SYSTEM_STRUCT_H
#define BOOKING_SYSTEM_STRUCT_H

#define MAX_USER_LEN  10
#define MAX_PASS_LEN  20
#define ORE 12
#define MAX_PRENOTAZIONI_ATTIVE 3

#include <stdbool.h>
#include <stdio.h>

//=================================
// 1. ENUMERATORI
//=================================

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

    OP_ADM_LISTA_ATTESA,
    OP_ADM_APPROVA_PRENOTAZ,
    OP_ADM_RIFIUTA_PRENOTAZ,

    OP_ESCI
} op_cliente_t;

typedef enum {
    ESITO_KO = 0,
    ESITO_OK = 1
} esito_t;

//==================================
// 2. STRUTTURE DATI (INVARIATE)
//==================================

typedef struct {
    char username[MAX_USER_LEN + 1];
    char password[MAX_PASS_LEN + 1];
    bool isAdmin;   
} utente_t;

typedef struct {
    int id_risorsa;             // Identificatore unico dell'aula (es. 101)
    char nome[30];              // Es. "Aula Magna"
    int capienza;               // Es. 50
} risorsa_aula_t;

//TODO cambiare le impostazioni delle ore
typedef struct {
    int id_prenotazione;
    int id_risorsa;
    char data[11]; // Formato: YYYY-MM-DD
    char ora_inizio[6]; // Formato: HH:MM
    char ora_fine[6]; // Formato: HH:MM
    utente_t utente;
    enum stato_prenotazione stato;
} prenotazione_t;

//==========================================
// 3. MESSAGGIO DI RICHIESTA Client-> Server
//==========================================

typedef struct {
    op_cliente_t operazione;
    utente_t utente;
} richiesta_t;

typedef struct {
    int id_risorsa;
    char data[11];      // Formato: YYYY-MM-DD
    char ora_inizio[6]; // Formato: HH:MM
    char ora_fine[6];   // Formato: HH:MM
} richiesta_prenotazione_t;

typedef struct {
    int id_prenotazione;
    int id_risorsa; // permette al server di individuare subito il file dell'aula
} richiesta_gestione_prenotazione_t;

//==========================================
// 4. MESSAGGIO DI RISPOSTA Server -> Client
//==========================================

typedef struct {
    esito_t esito;              // ESITO_OK o ESITO_KO
    op_cliente_t operazione;    // Riferimento all'operazione
    char messaggio[96];         // Es. "Login effettuato" o "Password errata"
    int num_elementi;           // Quanti elementi ci sono dopo (es. 7 aule, 1 utente, 0 se errore)
    size_t payload_size;        // Dimensione esatta in BYTE del payload che segue
} risposta_header_t;


#endif // BOOKING_SYSTEM_STRUCT_H