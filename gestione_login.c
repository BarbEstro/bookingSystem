#include "gestione_login.h"
#include "myfile.h"
#include "protocollo_login.h"
#include <stdbool.h>

#define DIMENSIONE_UTENZA (MAX_USER_LEN + MAX_PASS_LEN + 3) // +3 per il separatore e il terminatore di stringa

static bool controllo_username(const char* username){
    bool esito = false;
    FILE* file = aprireFile(UTENTI_FILE,"r");
    char buffer[DIMENSIONE_UTENZA];
    while(leggereRigaFile(file,buffer, sizeof(buffer))){
        char fileUsername[MAX_USER_LEN + 1];
        sscanf(buffer, "%[^-]-",fileUsername);
        if(strcmp(username,fileUsername) == 0){
            closeFile(file);
            esito = true;
            break;
        }
    }

    return esito;
    
}

int verificaCredenziali(const char *username, const char *password) {
    FILE* file = aprireFile(UTENTI_FILE, "r");
    char buffer[DIMENSIONE_UTENZA];
    while(leggereRigaFile(file, buffer, sizeof(buffer))) {
        char fileUsername[MAX_USER_LEN + 1];
        char filePassword[MAX_PASS_LEN + 1];
        int isAdmin;
        sscanf(buffer, "%[^-]-%[^-]-%d", fileUsername, filePassword, &isAdmin);
        if(strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
            closeFile(file);
            return isAdmin; // Ritorna 1 se è admin, 0 altrimenti
        }
    }
    closeFile(file);
    return -1; // Credenziali non valide
}

bool registraUtente(const char *username, const char *password, int isAdmin) {
    FILE* file = openFile(UTENTI_FILE, "a");
    char nuova_utenza[DIMENSIONE_UTENZA];
    snprintf(nuova_utenza, sizeof(nuova_utenza), "%s-%s-%d\n", username, password, isAdmin);
    if(controllo_username(nuova_utenza)){
        return false;
    }

    scriviFile(file, nuova_utenza);
    closeFile(file);

    return true;
}