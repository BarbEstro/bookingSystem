#ifndef GESTIONE_LOGIN_H
#define GESTIONE_LOGIN_H

#define UTENTI_FILE "utenti.txt"

#include <stdbool.h>

int verificaCredenziali(const char *username, const char *password);
bool registraUtente(const char *username, const char *password, int isAdmin);

#endif