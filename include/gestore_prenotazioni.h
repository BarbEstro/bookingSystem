#ifndef GESTORE_PRENOTAZIONI_H
#define GESTORE_PRENOTAZIONI_H

#include "booking_system_struct.h"
#include <stdbool.h>

// Inizializza la mappa in RAM leggendo aule e prenotazioni esistenti da file
//bool init_gestore_prenotazioni(const char* file_risorse, const char* file_prenotazioni);

// Verifica e aggiunge una nuova prenotazione (ritorna true se OK, false con messaggio se KO)
bool inserisci_prenotazione(richiesta_prenotazione_t dati_p, utente_t utente, char* messaggio_esito);

// Ritorna le aule in RAM (utile per OP_CLI_LISTA_RISORSE senza rileggere il file)
//risorsa_aula_t* ottieni_catalogo_aule(size_t* num_aule_out);

// Salva tutte le prenotazioni della RAM su file binario e libera la memoria allocata
//void chiudi_e_salva_gestore_prenotazioni(const char* file_prenotazioni);

#endif // GESTORE_PRENOTAZIONI_H