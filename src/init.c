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

void inizializza_risorse_disponibilita() {
  risorsa_aula_t aule[] = {{101, "Aula Magna", 150},
                              {102, "Laboratorio Informatica A", 30},
                              {103, "Laboratorio Informatica B", 25},
                              {201, "Aula Studio 1", 40},
                              {202, "Aula Studio 2", 20},
                              {301, "Sala Conferenze", 80},
                              {302, "Aula Seminari", 15}};
                              //Aggiungi aula

  size_t n_risorse = sizeof(aule) / sizeof(aule[0]);

  // Esempio di salvataggio su binario "dati/risorse.bn"
  FILE* file_risorse = fopen("dati/risorse.bn", "wb");
  if (file_risorse != NULL) {
    fwrite(aule, sizeof(risorsa_aula_t), n_risorse, file_risorse);
    fclose(file_risorse);
    printf("[OK] Salvate %zu risorse in dati/risorse.bn\n", n_risorse);
  }

  FILE* file_disponibilita = fopen("dati/calendario.bn", "wb");
  if(file_disponibilita != NULL){
    disponibilita_aula_t calendario[n_risorse];
    for(int i = 0; i < n_risorse; i++){
      calendario[i].aula = aule[i];
      for(int j = 0; j < ORE; j++){
        calendario->ore_stato[j] = 0;
      }
    }
    fwrite(calendario,sizeof(disponibilita_aula_t),n_risorse,file_disponibilita);
    fclose(file_disponibilita);
  }
}

int main() {
    inizializzazione_cartella();
    creazione_file_utenza();
    inizializza_risorse_disponibilita();
}