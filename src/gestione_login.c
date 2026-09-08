#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "gestione_login.h"
#include "booking_system_struct.h"
#include "debug.h"

static bool controllo_username(const char* username);
static bool verifica_esistenza_file_username(const char primo_carattere, char* path, size_t path_size);
static char* crea_path_file_username(char* path, int size);

utente_t* crea_utente(const char *username, const char *password, bool isAdmin) {
    utente_t* nuovo_utente = malloc(sizeof(utente_t));
    if (nuovo_utente == NULL) {
        return NULL; // Errore di allocazione
    }
    strncpy(nuovo_utente->username, username, MAX_USER_LEN);
    strncpy(nuovo_utente->password, password, MAX_PASS_LEN);
    nuovo_utente->isAdmin = isAdmin;

    return nuovo_utente;
}

utente_t* verificaCredenziali(const char *username, const char *password) {
    char primo_carattere = username[0];
    char path[32];

    if (verifica_esistenza_file_username(primo_carattere, path, sizeof(path))) {
        FILE* file = fopen(path, "rb");
        if (file != NULL) {
            utente_t* utente = malloc(sizeof(utente_t));
            if (utente == NULL) {
                fclose(file);
                return NULL;
            }

            // Il ciclo legge un utente alla volta e si arresta da solo a fine file (EOF)
            while (fread(utente, sizeof(utente_t), 1, file) == 1) {
                if (strcmp(utente->username, username) == 0 && strcmp(utente->password, password) == 0) {
                    fclose(file);
                    return utente; // Credenziali corrette (memoria restituita al chiamante)
                }
            }

            // Se non trova corrispondenze, libera la memoria ed evita memory leak
            free(utente);
            fclose(file);
        }
    }
    
    return NULL; // Utente non trovato o password errata
}

bool registraUtente(utente_t* utente) {
    bool esito = false;
    char path[32];
    LOG("Controllo se esiste un username simile");
    if(!controllo_username(utente->username)) {
        if(!verifica_esistenza_file_username(utente->username[0], path, sizeof(path))) {
            crea_path_file_username(path, sizeof(path));
        }
        FILE* file = fopen(path, "ab");
        size_t written = fwrite(utente, sizeof(utente_t), 1, file);
        esito = true;
        fclose(file);
    }

    return esito;
}

static bool controllo_username(const char* username){
    bool esito = false;
    char primo_carattere = username[0];
    char path[32];

    if(verifica_esistenza_file_username(primo_carattere, path, sizeof(path))){
        FILE* file = fopen(path, "rb");
        utente_t* utente = malloc(sizeof(utente_t));
        while(fread(utente, sizeof(utente_t), 1, file) == 1){
            if(strcmp(utente->username, username) == 0){
                esito = true;
                LOG("USERNAME TROVATO");
                break;
            }
        }
        free(utente);
        fclose(file);
    }

    return esito;
    
}

static bool verifica_esistenza_file_username(const char primo_carattere, char* path, size_t path_size) {
    LOG("verifica esistenza file");
    snprintf(path,path_size, "dati/utenti_%c.bn", primo_carattere);
    LOG("File da controllare : %s", path);
    return access(path, F_OK) == 0; // Verifica se il file esiste
}

static char* crea_path_file_username(char* path, int size){
    FILE* file = fopen(path, "wb"); // Crea il file se non esiste
    fclose(file);
    return path;
}