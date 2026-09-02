#include "gestore_prenotazioni.h"
#include "gestione_operazioni_server.h" // Per carica_risorse_da_file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



bool inserisci_prenotazione(richiesta_prenotazione_t dati_p, utente_t utente, char* messaggio_esito) {
    if (!e_data_futura(dati_p.data)) {
        strcpy(messaggio_esito, "Errore: La data deve essere futura.");
        return false;
    }

    // Trova l'aula
    mappa_aula_t* target = NULL;
    for (size_t i = 0; i < totale_aule; i++) {
        if (mappa[i].id_risorsa == dati_p.id_risorsa) {
            target = &mappa[i];
            break;
        }
    }

    if (target == NULL) {
        strcpy(messaggio_esito, "Errore: ID Aula non valido.");
        return false;
    }

    if (ha_conflitto_orario(target->testa_prenotazioni, dati_p.data, dati_p.ora_inizio, dati_p.ora_fine)) {
        strcpy(messaggio_esito, "Errore: Aula gia' occupata in questa fascia oraria.");
        return false;
    }

    // Creazione nuova prenotazione
    nodo_prenotazione_t* nuovo = malloc(sizeof(nodo_prenotazione_t));
    nuovo->dato.id_prenotazione = progressivo_id_prenotazione++;
    nuovo->dato.id_risorsa = dati_p.id_risorsa;
    strcpy(nuovo->dato.data, dati_p.data);
    strcpy(nuovo->dato.ora_inizio, dati_p.ora_inizio);
    strcpy(nuovo->dato.ora_fine, dati_p.ora_fine);
    nuovo->dato.utente = utente;
    nuovo->dato.stato = ATTESA; //[cite: 2]

    nuovo->next = target->testa_prenotazioni;
    target->testa_prenotazioni = nuovo;

    strcpy(messaggio_esito, "Prenotazione registrata con successo!");
    return true;
}