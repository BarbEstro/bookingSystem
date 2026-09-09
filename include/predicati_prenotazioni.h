#ifndef PREDICATI_PRENOTAZIONI_H
#define PREDICATI_PRENOTAZIONI_H

#include <stdbool.h>
#include "mappa_prenotazioni.h" 

// Predicato: la prenotazione appartiene all'utente il cui username e' passato come contesto
bool predicato_per_username(const prenotazione_t* p, void* contesto);

// Predicato: nessun filtro, accetta tutte le prenotazioni (usato dall'admin)
bool predicato_per_stato(const prenotazione_t* p, void* contesto);

#endif