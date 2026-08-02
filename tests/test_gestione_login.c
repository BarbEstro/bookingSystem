#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gestione_login.h"
#include "booking_system_struct.h"

// Funzione helper per creare la cartella dati se non esiste
void prepara_ambiente() {
    mkdir("dati", 0777);
    remove("dati/utenti_m.bn");
}

void test_crea_utente() {
    utente_t* u = crea_utente("mario", "pass123", false);
    assert(u != NULL);
    assert(strcmp(u->username, "mario") == 0);
    assert(strcmp(u->password, "pass123") == 0);
    assert(u->isAdmin == false);
    free(u);
    printf("[OK] test_crea_utente passato.\n");
}

void test_registrazione_e_login() {
    utente_t* u = crea_utente("mario", "pass123", false);
    
    // Test Scrittura su file
    bool reg_esito = registraUtente(u);
    assert(reg_esito == true);

    // Test Login con credenziali corrette
    utente_t* u_logged = verificaCredenziali("mario", "pass123");
    assert(u_logged != NULL);
    assert(strcmp(u_logged->username, "mario") == 0);

    // Test Login con password errata
    utente_t* u_errato = verificaCredenziali("mario", "wrongpass");
    assert(u_errato == NULL);

    // Test Login utente inesistente
    utente_t* u_fantasma = verificaCredenziali("luigi", "pass123");
    assert(u_fantasma == NULL);

    free(u);
    free(u_logged);
    printf("[OK] test_registrazione_e_login passato.\n");
}

int main() {
    printf("=== AVVIO TEST GESTIONE LOGIN ===\n");
    prepara_ambiente();
    
    test_crea_utente();
    test_registrazione_e_login();

    printf("=== TUTTI I TEST SONO SUPERATI! ===\n");
    return 0;
}