#ifndef GESTIONE_LOGIN_H
#define GESTIONE_LOGIN_H

#define UTENTI_FILE "utenti.txt"

int verificaCredenziali(const char *username, const char *password);
int registraUtente(const char *username, const char *password, int isAdmin);

#endif