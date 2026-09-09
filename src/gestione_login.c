#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>

#include "gestione_login.h"
#include "booking_system_struct.h"
#include "gestione_lock.h"
#include "debug.h"

#define FILE_LOCK_UTENTI "dati/utenti.lock"

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
    utente_t* risultato = NULL;

    int lock_fd = acquisisci_lock(LOCK_SH, FILE_LOCK_UTENTI);
    if (lock_fd < 0) return NULL;

    if (verifica_esistenza_file_username(primo_carattere, path, sizeof(path))) {
        FILE* file = fopen(path, "rb");
        if (file != NULL) {
            utente_t* utente = malloc(sizeof(utente_t));
            if (utente != NULL) {
                while (fread(utente, sizeof(utente_t), 1, file) == 1) {
                    if (strcmp(utente->username, username) == 0 && strcmp(utente->password, password) == 0) {
                        risultato = utente;
                        break;
                    }
                }
                if (risultato == NULL) free(utente);
            }
            fclose(file);
        }
    }

    rilascia_lock(lock_fd);
    return risultato;
}

bool registraUtente(utente_t* utente) {
    bool esito = false;
    char path[32];

    int lock_fd = acquisisci_lock(LOCK_EX, FILE_LOCK_UTENTI);
    if (lock_fd < 0) return false;

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

    rilascia_lock(lock_fd);
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