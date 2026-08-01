#include "gestione_login.h"
#include "myfile.h"
#include "protocollo_login.h"
#include <stdbool.h>
#include <string.h>

static bool controllo_username(const char* username);

int verificaCredenziali(const char *username, const char *password) {
    FILE* file = aprireFile(UTENTI_FILE, "r");
    char buffer[PACKET_SIZE];
    int risultato = -1;
    while(leggereRigaFile(file, buffer, sizeof(buffer))) {
        char fileUsername[MAX_USER_LEN + 1];
        char filePassword[MAX_PASS_LEN + 1];
        int isAdmin;
        sscanf(buffer, "%d-%[^-]-%[^-]", &isAdmin, fileUsername, filePassword);
        if(strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
            risultato = isAdmin; // Ritorna 1 se è admin, 0 altrimenti
            break;
        }
    }
    chiusuraFile(file);
    return risultato; // Credenziali non valide
}

bool registraUtente(const char *username, const char *password, int isAdmin) {
    FILE* file = aprireFile(UTENTI_FILE, "a");
    char nuova_utenza[PACKET_SIZE];
    snprintf(nuova_utenza, sizeof(nuova_utenza), "%d-%s-%s\n",isAdmin, username,password);
    if(controllo_username(nuova_utenza)){
        return false;
    }

    if(!scritturaFile(file, nuova_utenza)){
        chiusuraFile(file);
        return false;
    }

    chiusuraFile(file);

    return true;
}

static bool controllo_username(const char* username){
    bool esito = false;
    FILE* file = aprireFile(UTENTI_FILE,"r");
    char buffer[PACKET_SIZE];
    while(leggereRigaFile(file,buffer, sizeof(buffer))){
        char fileUsername[MAX_USER_LEN + 1];
        sscanf(buffer, "%*d-%[^-]", fileUsername);
        if(strcmp(username,fileUsername) == 0){
            chiusuraFile(file);
            esito = true;
            break;
        }
    }

    return esito;
    
}