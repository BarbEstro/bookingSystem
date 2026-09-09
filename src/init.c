#include <booking_system_struct.h>
#include <gestione_login.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static size_t contatore_id_risorse(size_t* count) {
  *count = *count + 1;
  return *count;
}

//FIXME aggiunta nella relazione che non ho usato un sistema unix per semplicità
static void inizializzazione_cartella() {
  system("rm -rf dati");  // Il flag -r cancella RICORSIVAMENTE anche la cartella!
  system("mkdir dati");  // Ora la ricrea vuota da zero
}

static void creazione_file_utenza() {
  utente_t utenti[] = {{"pippo", "pappo", false},
                       {"pluto", "plato", false},
                       {"admin", "admin123", true}};
  // aggiungi utente

  int n_utenti = sizeof(utenti) / sizeof(utente_t);

  for (int i = 0; i < n_utenti; i++) {
    registraUtente(&utenti[i]);
  }
}

static void inizializza_risorse_disponibilita() {
  size_t n_risorse = 0;
  risorsa_aula_t aule[] = {
      {contatore_id_risorse(&n_risorse), "Aula Magna", 150},
      {contatore_id_risorse(&n_risorse), "Laboratorio Informatica A", 30},
      {contatore_id_risorse(&n_risorse), "Laboratorio Informatica B", 25},
      {contatore_id_risorse(&n_risorse), "Aula Studio 1", 40},
      {contatore_id_risorse(&n_risorse), "Aula Studio 2", 20},
      {contatore_id_risorse(&n_risorse), "Sala Conferenze", 80},
      {contatore_id_risorse(&n_risorse), "Aula Seminari", 15}
       // Aggiungi aula
    };

  // Esempio di salvataggio su binario "dati/risorse.bn"
  FILE* file_risorse = fopen("dati/risorse.bn", "wb");
  if (file_risorse != NULL) {
    fwrite(aule, sizeof(risorsa_aula_t), n_risorse, file_risorse);
    fclose(file_risorse);
    printf("[OK] Salvate %zu risorse in dati/risorse.bn\n", n_risorse);
  }
}

int main() {
  inizializzazione_cartella();
  creazione_file_utenza();
  inizializza_risorse_disponibilita();
}