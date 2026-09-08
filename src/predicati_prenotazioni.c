#include "predicati_prenotazioni.h"
#include <string.h>

bool predicato_per_username(const prenotazione_t* p, void* contesto) {
    const char* username = (const char*)contesto;
    return strcmp(p->utente.username, username) == 0;
}

bool predicato_per_stato(const prenotazione_t* p, void* contesto) {
    const char* stato = (const char*)contesto;
    return p->stato == stato;
}