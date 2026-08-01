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

typedef struct {
    char username[MAX_USER_LEN + 1];
    char password[MAX_PASS_LEN + 1];
    bool isAdmin;
    char *file_path_prenotazioni;
    
} utente_t;

typedef struct {
    int id_prenotazione;
    char data[11]; // Formato: YYYY-MM-DD
    char ora_inizio[6]; // Formato: HH:MM
    char ora_fine[6]; // Formato: HH:MM
    char richiedente[MAX_USER_LEN + 1];
    enum stato_prenotazione stato;
} prenotazione_t;

#endif // BOOKING_SYSTEM_STRUCT_H