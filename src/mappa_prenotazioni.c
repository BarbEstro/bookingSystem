#include "mappa_prenotazioni.h"
#include <stdlib.h>

mappa_t* crea_mappa(const risorsa_aula_t* aule, size_t num_aule){
    if(aule == NULL || num_aule == 0) return NULL;

    mappa_t* mappa = malloc(sizeof(mappa_t));
    if(mappa == NULL) return NULL;

   
    mappa->num_aule = num_aule;
    mappa->bucket = malloc(num_aule * sizeof(bucket_aula_t));
    if(mappa->bucket == NULL){
        free(mappa);
        return NULL;
    }

    for(size_t i = 0; i < num_aule; i++){
        mappa->bucket[i].id_risorsa = aule[i].id_risorsa;
        mappa->bucket[i].testa = NULL;
    }

    return mappa;
    
}

bool mappa_inserisci(mappa_t* mappa, int id_aula, prenotazione_t p){
    if(mappa == NULL) return false;

    for(size_t i = 0; i < mappa->num_aule; i++){
        if(mappa->bucket[i].id_risorsa == id_aula){
            nodo_prenotazione_t* nuovo_nodo = malloc(sizeof(nodo_prenotazione_t));
            if(nuovo_nodo == NULL) return false;

            nuovo_nodo->dato = p;
            nuovo_nodo->next = mappa->bucket[i].testa;
            mappa->bucket[i].testa = nuovo_nodo;
            return true;
        }
    }
    return false; // Aula non trovata
}

bool mappa_rimuovi_prenotazione(mappa_t* mappa, int id_aula, int id_prenotazione){
    if(mappa == NULL) return false;

    for(size_t i = 0; i < mappa->num_aule; i++){
        if(mappa->bucket[i].id_risorsa == id_aula){
            nodo_prenotazione_t* current = mappa->bucket[i].testa;
            nodo_prenotazione_t* prev = NULL;

            while(current != NULL){
                if(current->dato.id_prenotazione == id_prenotazione){
                    if(prev == NULL){
                        mappa->bucket[i].testa = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    free(current);
                    return true;
                }
                prev = current;
                current = current->next;
            }
            return false; // Prenotazione non trovata
        }
    }
    return false; // Aula non trovata
}

void libera_mappa(mappa_t* mappa){
    if(mappa == NULL) return;

    for(size_t i = 0; i < mappa->num_aule; i++){
        nodo_prenotazione_t* current = mappa->bucket[i].testa;
        while(current != NULL){
            nodo_prenotazione_t* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(mappa->bucket);
    free(mappa);
}