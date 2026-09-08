#ifndef GESTIONE_LOGIN_H
#define GESTIONE_LOGIN_H

#include <stdbool.h>
#include "booking_system_struct.h"

utente_t* crea_utente(const char *username, const char *password, bool isAdmin);
utente_t* verificaCredenziali(const char *username, const char *password);
bool registraUtente(utente_t* utente);

#endif