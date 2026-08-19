#include <booking_system_struct.h>
#include <gestione_login.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void inizializzazione_cartella() {
  system("rm -rf dati");  // Il flag -r cancella RICORSIVAMENTE anche la cartella!
  system("mkdir dati");  // Ora la ricrea vuota da zero
}

void creazione_file_utenza() {
  utente_t utenti[] = {{"pippo", "pappo", false},
                       {"pluto", "plato", false},
                       {"admin", "admin123", true}};
                       //aggiungi utente

  int n_utenti = sizeof(utenti) / sizeof(utente_t);

  for (int i = 0; i < n_utenti; i++) {
    registraUtente(&utenti[i]);
  }
}

void inizializza_risorse() {
  risorsa_aula_t risorse[] = {{101, "Aula Magna", 150},
                              {102, "Laboratorio Informatica A", 30},
                              {103, "Laboratorio Informatica B", 25},
                              {201, "Aula Studio 1", 40},
                              {202, "Aula Studio 2", 20},
                              {301, "Sala Conferenze", 80},
                              {302, "Aula Seminari", 15}};
                              //Aggiungi aula

  size_t n_risorse = sizeof(risorse) / sizeof(risorse[0]);

  // Esempio di salvataggio su binario "dati/risorse.bn"
  FILE* file = fopen("dati/risorse.bn", "wb");
  if (file != NULL) {
    fwrite(risorse, sizeof(risorsa_aula_t), n_risorse, file);
    fclose(file);
    printf("[OK] Salvate %zu risorse in dati/risorse.bn\n", n_risorse);
  }
}
int main() {
    inizializzazione_cartella();
    creazione_file_utenza();
    inizializza_risorse();
}