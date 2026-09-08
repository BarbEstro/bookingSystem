#include "mappa_prenotazioni.h"

#include <stdio.h>
#include <stdlib.h>

mappa_t* crea_mappa(const risorsa_aula_t* aule, size_t num_aule) {
  if (aule == NULL || num_aule == 0) return NULL;

  mappa_t* mappa = malloc(sizeof(mappa_t));
  if (mappa == NULL) return NULL;

  mappa->num_aule = num_aule;
  mappa->bucket = malloc(num_aule * sizeof(bucket_aula_t));
  if (mappa->bucket == NULL) {
    free(mappa);
    return NULL;
  }

  for (size_t i = 0; i < num_aule; i++) {
    mappa->bucket[i].id_risorsa = aule[i].id_risorsa;
    mappa->bucket[i].testa = NULL;
  }

  return mappa;
}

bool mappa_inserisci_prenotazione(mappa_t* mappa, int id_aula,prenotazione_t p) {
  if (mappa == NULL) return false;

  size_t indice = 0;
  bool trovato = false;
  for (size_t i = 0; i < mappa->num_aule; i++) {
    if (mappa->bucket[i].id_risorsa == id_aula) {
      indice = i;
      trovato = true;
      break;
    }
  }
  if (!trovato) return false;  // Aula inesistente

  nodo_prenotazione_t* nuovo_nodo = malloc(sizeof(nodo_prenotazione_t));
  if (nuovo_nodo == NULL) return false;
  nuovo_nodo->dato = p;
  nuovo_nodo->next = mappa->bucket[indice].testa;
  mappa->bucket[indice].testa = nuovo_nodo;

  return true;  // Inserimento riuscito
}

bool mappa_rimuovi_prenotazione(mappa_t* mappa, int id_aula,
                                int id_prenotazione) {
  if (mappa == NULL) return false;

  size_t indice = 0;
  bool trovato = false;
  for (size_t i = 0; i < mappa->num_aule; i++) {
    if (mappa->bucket[i].id_risorsa == id_aula) {
      indice = i;
      trovato = true;
      break;
    }
  }
  if (!trovato) return false;  // Aula inesistente

  nodo_prenotazione_t* current = mappa->bucket[indice].testa;
  nodo_prenotazione_t* prev = NULL;

  while (current != NULL) {
    if (current->dato.id_prenotazione == id_prenotazione) {
      if (prev == NULL) {
        mappa->bucket[indice].testa = current->next;
      } else {
        prev->next = current->next;
      }
      free(current);
      return true;
    }
    prev = current;
    current = current->next;
  }
  return false;
}

void libera_mappa(mappa_t* mappa) {
  if (mappa == NULL) return;

  for (size_t i = 0; i < mappa->num_aule; i++) {
    nodo_prenotazione_t* current = mappa->bucket[i].testa;
    while (current != NULL) {
      nodo_prenotazione_t* temp = current;
      current = current->next;
      free(temp);
    }
  }

  free(mappa->bucket);
  free(mappa);
}

nodo_prenotazione_t* mappa_ottieni_lista(mappa_t* mappa, int id_aula) {
  if (mappa == NULL) return NULL;

  for (size_t i = 0; i < mappa->num_aule; i++) {
    if (mappa->bucket[i].id_risorsa == id_aula) {
      return mappa->bucket[i].testa;
    }
  }
  return NULL;  // Aula non trovata
}

bool mappa_esiste_aula(mappa_t* mappa, int id_aula) {
  if (mappa == NULL) return false;

  for (size_t i = 0; i < mappa->num_aule; i++) {
    if (mappa->bucket[i].id_risorsa == id_aula) return true;
  }
  return false;
}

prenotazione_t* mappa_filtra_prenotazioni(mappa_t* mappa, prenotazione_predicato_t predicato, void* contesto, size_t* out_count) {
  if (out_count != NULL) *out_count = 0;
  if (mappa == NULL || predicato == NULL) return NULL;

  // 1. Primo passaggio: conta quante prenotazioni soddisfano il predicato
  size_t trovate = 0;
  for (size_t i = 0; i < mappa->num_aule; i++) {
    for (nodo_prenotazione_t* n = mappa->bucket[i].testa; n != NULL; n = n->next) {
      if (predicato(&n->dato, contesto)) trovate++;
    }
  }
  if (trovate == 0) return NULL;

  // 2. Secondo passaggio: alloca e copia i risultati
  prenotazione_t* risultati = malloc(trovate * sizeof(prenotazione_t));
  if (risultati == NULL) return NULL;

  size_t idx = 0;
  for (size_t i = 0; i < mappa->num_aule; i++) {
    for (nodo_prenotazione_t* n = mappa->bucket[i].testa; n != NULL; n = n->next) {
      if (predicato(&n->dato, contesto)) risultati[idx++] = n->dato;
    }
  }

  if (out_count != NULL) *out_count = trovate;
  return risultati;
}

void mappa_path_file_aula(int id_risorsa, char* buffer, size_t size) {
  snprintf(buffer, size, "dati/prenotazioni_aula_%d.bn", id_risorsa);
}

bool mappa_salva_prenotazione_su_file(prenotazione_t p) {
  char path[64];
  mappa_path_file_aula(p.id_risorsa, path, sizeof(path));

  FILE* file = fopen(path, "ab");
  if (file == NULL) return false;

  bool esito = fwrite(&p, sizeof(prenotazione_t), 1, file) == 1;
  fclose(file);
  return esito;
}

bool mappa_carica_da_file(mappa_t* mappa) {
  if (mappa == NULL) return false;

  char path[64];
  for (size_t i = 0; i < mappa->num_aule; i++) {
    int id_risorsa = mappa->bucket[i].id_risorsa;
    mappa_path_file_aula(id_risorsa, path, sizeof(path));

    FILE* file = fopen(path, "rb");
    if (file == NULL) continue;  // Nessuna prenotazione salvata per questa aula

    prenotazione_t p;
    while (fread(&p, sizeof(prenotazione_t), 1, file) == 1) {
      mappa_inserisci_prenotazione(mappa, id_risorsa, p);
    }
    fclose(file);
  }
  return true;
}