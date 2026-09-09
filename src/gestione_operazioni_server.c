#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <sys/file.h>
#include "gestione_operazioni_server.h"
#include "gestione_login.h"
#include "mappa_prenotazioni.h"
#include "predicati_prenotazioni.h"
#include "debug.h"

#define FILE_LOCK_PRENOTAZIONI "dati/prenotazioni.lock"

static void invia_elenco_prenotazioni(int client_sock, op_cliente_t operazione, prenotazione_t* risultati, size_t count);
static void ottieni_data_odierna(char *buffer, size_t size);
static void ottieni_ora_odierna(char *buffer, size_t size);
static bool valida_data_e_ora(const char *data_req, const char *ora_inizio, const char *ora_fine, char *msg_errore);
static bool verifica_disponibilita_aula(int id_aula, const char* data, const char* ora_inizio, const char* ora_fine);
static void analizza_prenotazioni_esistenti(mappa_t* mappa, const char* username, size_t* out_attive_utente, int* out_max_id);
static mappa_t* ricostruisci_mappa_da_file(mappa_t* mappa_originale);
static prenotazione_t* carica_prenotazioni_aula(int id_risorsa, size_t* out_count);
static bool salva_prenotazioni_aula(int id_risorsa, const prenotazione_t* prenotazioni, size_t count);
static bool si_sovrappongono(const prenotazione_t* a, const prenotazione_t* b);

void operazione_login(int client_sock, richiesta_t richiesta) {
    risposta_header_t header;
    header.operazione = OP_CLI_LOGIN;

    printf("Richiesta di login ricevuta: username=%s, password=%s\n", richiesta.utente.username, richiesta.utente.password); //
    utente_t* utente = verificaCredenziali(richiesta.utente.username, richiesta.utente.password); //
    
    if(utente != NULL) {
        header.esito = ESITO_OK; //
        strcpy(header.messaggio, "Login effettuato con successo"); //
        header.num_elementi = 1;
        header.payload_size = sizeof(utente_t); 

        // 1. Invio prima l'Header, poi l'Utente
        write(client_sock, &header, sizeof(header));
        write(client_sock, utente, header.payload_size);
        
        free(utente); // Libera la memoria allocata per l'utente
    } else {
        header.esito = ESITO_KO; //
        strcpy(header.messaggio, "USERNAME o PASSWORD errati"); //
        header.num_elementi = 0;
        header.payload_size = 0; // Nessun payload
        
        // 2. Mando solo l'header per notificare l'errore
        write(client_sock, &header, sizeof(header));
    }
}

void operazione_registrazione(int client_sock, richiesta_t richiesta) {
    risposta_header_t header;
    header.operazione = OP_CLI_REGISTRAZIONE;
    header.num_elementi = 0;
    header.payload_size = 0; // La registrazione non invia mai payload!

    printf("Richiesta di registrazione ricevuta: username=%s, password=%s, isAdmin=%d\n", richiesta.utente.username, richiesta.utente.password, richiesta.utente.isAdmin); //
    utente_t* nuovo_utente = crea_utente(richiesta.utente.username, richiesta.utente.password, richiesta.utente.isAdmin); //
    
    LOG("Fase di controllo utente"); //
    if (nuovo_utente != NULL){ //
        if (registraUtente(nuovo_utente)) { //
            LOG("Non esiste"); //
            header.esito = ESITO_OK; //
            strcpy(header.messaggio, "Registrazione effettuata"); //
        } else {
            LOG("esiste username"); //
            header.esito = ESITO_KO; //
            strcpy(header.messaggio, "Registrazione negata, username esistente"); //
        }
        free(nuovo_utente); // Libera la memoria allocata per il nuovo utente
    } else {
        header.esito = ESITO_KO; //
        strcpy(header.messaggio, "Errore interno server");
        printf("Errore nella creazione dell'utente.\n"); //
    }
    
    // Manda l'esito
    write(client_sock, &header, sizeof(header));
}

void operazione_invia_catalogo_aule(int client_sock, richiesta_t richiesta, risorsa_aula_t* aule, size_t num_aule) {

    risposta_header_t header;
    header.operazione = richiesta.operazione; // Usa automaticamente l'operazione della richiesta

    if (aule != NULL && num_aule > 0) {
        header.esito = ESITO_OK;
        strcpy(header.messaggio, "Elenco aule per selezione");
        header.num_elementi = (int)num_aule;
        header.payload_size = num_aule * sizeof(risorsa_aula_t); 
    } else {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Nessuna risorsa trovata sul server");
        header.num_elementi = 0;
        header.payload_size = 0;
    }

    // 1. Invio dell'header
    write(client_sock, &header, sizeof(header));

    // 2. Invio del payload con l'array di aule
    if (header.esito == ESITO_OK && header.payload_size > 0) {
        write(client_sock, aule, header.payload_size);
    }
}

void operazione_salva_prenotazione(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni) {
    richiesta_prenotazione_t dati_prenotazione;
    risposta_header_t header;
    header.operazione = OP_CLI_NUOVA_PRENOTAZ;
    header.num_elementi = 0;
    header.payload_size = 0;

    // 1. Legge il payload inviato dal client contenente la scelta dell'utente
    if (read(client_sock, &dati_prenotazione, sizeof(richiesta_prenotazione_t)) <= 0) {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Errore nella ricezione dei dati di prenotazione");
        write(client_sock, &header, sizeof(header));
        return;
    }

    // 2. Valida la data e l'orario della prenotazione
    //FIXME aggiungere nella relazione che suppongo che il cliente inserisca i dati corretti
    if (!valida_data_e_ora(dati_prenotazione.data, dati_prenotazione.ora_inizio, dati_prenotazione.ora_fine, header.messaggio)) {
        header.esito = ESITO_KO;
        write(client_sock, &header, sizeof(header));
        return;
    }

    // 3. Sezione critica: un solo processo alla volta puo' leggere/scrivere le prenotazioni
    int lock_fd = acquisisci_lock(LOCK_EX, FILE_LOCK_PRENOTAZIONI);
    if (lock_fd < 0) {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Errore interno del server");
        write(client_sock, &header, sizeof(header));
        return;
    }

    size_t prenotazioni_attive_utente;
    int prossimo_id;
    analizza_prenotazioni_esistenti(mappa_prenotazioni, richiesta.utente.username, &prenotazioni_attive_utente, &prossimo_id);

    if (prenotazioni_attive_utente >= MAX_PRENOTAZIONI_ATTIVE) {
        rilascia_lock(lock_fd);
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Numero massimo di prenotazioni attive raggiunto");
        write(client_sock, &header, sizeof(header));
        return;
    }

    if (!mappa_esiste_aula(mappa_prenotazioni, dati_prenotazione.id_risorsa)) {
        rilascia_lock(lock_fd);
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Aula inesistente");
        write(client_sock, &header, sizeof(header));
        return;
    }

    if (!verifica_disponibilita_aula(dati_prenotazione.id_risorsa, dati_prenotazione.data, dati_prenotazione.ora_inizio, dati_prenotazione.ora_fine)) {
        rilascia_lock(lock_fd);
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Aula gia' occupata nella fascia oraria richiesta");
        write(client_sock, &header, sizeof(header));
        return;
    }

    prenotazione_t nuova_p;
    nuova_p.id_prenotazione = prossimo_id;
    nuova_p.id_risorsa = dati_prenotazione.id_risorsa;
    strcpy(nuova_p.data, dati_prenotazione.data);
    strcpy(nuova_p.ora_inizio, dati_prenotazione.ora_inizio);
    strcpy(nuova_p.ora_fine, dati_prenotazione.ora_fine);
    nuova_p.stato = ATTESA;
    nuova_p.utente = richiesta.utente;

    mappa_salva_prenotazione_su_file(nuova_p);

    rilascia_lock(lock_fd);

    header.esito = ESITO_OK;
    strcpy(header.messaggio, "Prenotazione inviata con successo!");

    // 4. Risposta al client
    write(client_sock, &header, sizeof(header));
}

void operazione_lista_mie_prenotazioni(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni) {
    int lock_fd = acquisisci_lock(LOCK_SH, FILE_LOCK_PRENOTAZIONI);
    mappa_t* mappa_fresca = ricostruisci_mappa_da_file(mappa_prenotazioni);
    size_t count = 0;
    prenotazione_t* risultati = mappa_filtra_prenotazioni(mappa_fresca, predicato_per_username, richiesta.utente.username, &count);
    rilascia_lock(lock_fd);
    libera_mappa(mappa_fresca);
    invia_elenco_prenotazioni(client_sock, OP_CLI_MIE_PRENOTAZ, risultati, count);
}

void operazione_lista_attesa_prenotazioni(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni) {
    (void)richiesta;
    enum stato_prenotazione stato = ATTESA;
    int lock_fd = acquisisci_lock(LOCK_SH, FILE_LOCK_PRENOTAZIONI);
    mappa_t* mappa_fresca = ricostruisci_mappa_da_file(mappa_prenotazioni);
    size_t count = 0;
    prenotazione_t* risultati = mappa_filtra_prenotazioni(mappa_fresca, predicato_per_stato, &stato, &count);
    rilascia_lock(lock_fd);
    libera_mappa(mappa_fresca);
    invia_elenco_prenotazioni(client_sock, OP_ADM_LISTA_ATTESA, risultati, count);
}

void operazione_gestisci_prenotazione(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni) {
    (void)mappa_prenotazioni; // lo stato condiviso vive nei file per-aula, non nella RAM del processo

    richiesta_gestione_prenotazione_t dati;
    risposta_header_t header;
    header.operazione = richiesta.operazione;
    header.num_elementi = 0;
    header.payload_size = 0;

    if (read(client_sock, &dati, sizeof(richiesta_gestione_prenotazione_t)) <= 0) {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Errore nella ricezione dei dati");
        write(client_sock, &header, sizeof(header));
        return;
    }

    // Sezione critica: nessun altro processo puo' leggere/scrivere le prenotazioni in questo momento
    int lock_fd = acquisisci_lock(LOCK_EX, FILE_LOCK_PRENOTAZIONI);
    if (lock_fd < 0) {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Errore interno del server");
        write(client_sock, &header, sizeof(header));
        return;
    }

    size_t count = 0;
    prenotazione_t* prenotazioni = carica_prenotazioni_aula(dati.id_risorsa, &count);

    int indice_target = -1;
    for (size_t i = 0; i < count; i++) {
        if (prenotazioni[i].id_prenotazione == dati.id_prenotazione) {
            indice_target = (int)i;
            break;
        }
    }

    if (indice_target < 0 || prenotazioni[indice_target].stato != ATTESA) {
        rilascia_lock(lock_fd);
        free(prenotazioni);
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Prenotazione non trovata o gia' gestita");
        write(client_sock, &header, sizeof(header));
        return;
    }

    if (richiesta.operazione == OP_ADM_APPROVA_PRENOTAZ) {
        prenotazioni[indice_target].stato = APPROVATA;
        // Chi era in attesa e si sovrapponeva alla prenotazione ora approvata non e' piu' realizzabile
        for (size_t i = 0; i < count; i++) {
            if ((int)i == indice_target) continue;
            if (prenotazioni[i].stato == ATTESA && si_sovrappongono(&prenotazioni[i], &prenotazioni[indice_target])) {
                prenotazioni[i].stato = RIFIUTATA;
            }
        }
        strcpy(header.messaggio, "Prenotazione approvata");
    } else {
        prenotazioni[indice_target].stato = RIFIUTATA;
        strcpy(header.messaggio, "Prenotazione rifiutata");
    }

    bool salvato = salva_prenotazioni_aula(dati.id_risorsa, prenotazioni, count);
    rilascia_lock(lock_fd);
    free(prenotazioni);

    header.esito = salvato ? ESITO_OK : ESITO_KO;
    if (!salvato) strcpy(header.messaggio, "Errore nel salvataggio della prenotazione");
    write(client_sock, &header, sizeof(header));
}

// Invia al client l'array 'risultati' (gia' filtrato) come risposta a 'operazione'
static void invia_elenco_prenotazioni(int client_sock, op_cliente_t operazione, prenotazione_t* risultati, size_t count) {
    risposta_header_t header;
    header.operazione = operazione;

    if (risultati != NULL && count > 0) {
        header.esito = ESITO_OK;
        strcpy(header.messaggio, "Elenco prenotazioni");
        header.num_elementi = (int)count;
        header.payload_size = count * sizeof(prenotazione_t);
    } else {
        header.esito = ESITO_KO;
        strcpy(header.messaggio, "Nessuna prenotazione trovata");
        header.num_elementi = 0;
        header.payload_size = 0;
    }

    write(client_sock, &header, sizeof(header));
    if (header.esito == ESITO_OK) {
        write(client_sock, risultati, header.payload_size);
    }
    free(risultati);
}

// Popola 'buffer' con la data odierna nel formato "YYYY-MM-DD" (es. "2026-09-03")
static void ottieni_data_odierna(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

// Popola 'buffer' con l'ora attuale nel formato "HH:MM" (es. "14:30")
static void ottieni_ora_odierna(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%H:%M", tm_info);
}

static bool valida_data_e_ora(const char *data_req, const char *ora_inizio, const char *ora_fine, char *msg_errore) {
    char data_oggi[11];
    char ora_oggi[6];

    ottieni_data_odierna(data_oggi, sizeof(data_oggi));
    ottieni_ora_odierna(ora_oggi, sizeof(ora_oggi));

    // 1. Controllo coerenza orario: ora_inizio deve precedere ora_fine
    if (strcmp(ora_inizio, ora_fine) >= 0) {
        strcpy(msg_errore, "Errore: L'ora di inizio deve essere precedente all'ora di fine.");
        return false;
    }

    // 2. Controllo Data: non puo essere nel passato
    int cmp_data = strcmp(data_req, data_oggi);
    
    if (cmp_data < 0) {
        strcpy(msg_errore, "Errore: Impossibile prenotare per una data passata.");
        return false;
    }


    // 3. Se la data e OGGI, l'ora di inizio non puo essere gia passata
    if (cmp_data == 0) {
        if (strcmp(ora_inizio, ora_oggi) <= 0) {
            strcpy(msg_errore, "Errore: Per oggi l'ora di inizio deve essere successiva all'ora attuale.");
            return false;
        }
    }

    // 4. Controllo fascia oraria consentita: 08:00 - 19:00
    if (strcmp(ora_inizio, "08:00") < 0 || strcmp(ora_fine, "19:00") > 0) {
      strcpy(msg_errore,
             "Errore: Le prenotazioni sono consentite solo tra le 08:00 e le "
             "19:00.");
      return false;
    }
    return true; // Tutti i controlli sono superati
}

static bool verifica_disponibilita_aula(int id_aula, const char* data, const char* ora_inizio, const char* ora_fine) {
    char path[64];
    mappa_path_file_aula(id_aula, path, sizeof(path));

    FILE* file = fopen(path, "rb");
    if (file == NULL) return true;  // Nessuna prenotazione esistente per questa aula

    bool disponibile = true;
    prenotazione_t p;
    while (disponibile && fread(&p, sizeof(prenotazione_t), 1, file) == 1) {
        // Le prenotazioni rifiutate non occupano piu' la fascia oraria
        if (p.stato != RIFIUTATA && p.stato != ATTESA && strcmp(p.data, data) == 0) {
            // Controllo sovrapposizione oraria
            if (!(strcmp(ora_fine, p.ora_inizio) <= 0 || strcmp(ora_inizio, p.ora_fine) >= 0)) {
                disponibile = false;
            }
        }
    }
    fclose(file);
    return disponibile;
}

// Scansiona i file di tutte le aule per contare le prenotazioni attive di 'username'
// e determinare il prossimo id_prenotazione univoco da assegnare (max esistente + 1)
static void analizza_prenotazioni_esistenti(mappa_t* mappa, const char* username, size_t* out_attive_utente, int* out_max_id) {
    size_t attive = 0;
    int max_id = 0;
    char path[64];

    for (size_t i = 0; i < mappa->num_aule; i++) {
        mappa_path_file_aula(mappa->bucket[i].id_risorsa, path, sizeof(path));
        FILE* file = fopen(path, "rb");
        if (file == NULL) continue;

        prenotazione_t p;
        while (fread(&p, sizeof(prenotazione_t), 1, file) == 1) {
            if (p.id_prenotazione > max_id) max_id = p.id_prenotazione;
            if (strcmp(p.utente.username, username) == 0 && p.stato == ATTESA) attive++;
        }
        fclose(file);
    }

    if (out_attive_utente != NULL) *out_attive_utente = attive;
    if (out_max_id != NULL) *out_max_id = max_id + 1;
}

// Ricostruisce da file una mappa temporanea con le stesse aule di 'mappa_originale',
// da usare per avere una vista sempre aggiornata durante le operazioni di sola lettura
static mappa_t* ricostruisci_mappa_da_file(mappa_t* mappa_originale) {
    risorsa_aula_t* aule_tmp = malloc(mappa_originale->num_aule * sizeof(risorsa_aula_t));
    if (aule_tmp == NULL) return NULL;
    for (size_t i = 0; i < mappa_originale->num_aule; i++) {
        aule_tmp[i].id_risorsa = mappa_originale->bucket[i].id_risorsa;
    }

    mappa_t* mappa_fresca = crea_mappa(aule_tmp, mappa_originale->num_aule);
    free(aule_tmp);
    if (mappa_fresca != NULL) mappa_carica_da_file(mappa_fresca);
    return mappa_fresca;
}

// Carica in un array dinamico tutte le prenotazioni salvate per una specifica aula
static prenotazione_t* carica_prenotazioni_aula(int id_risorsa, size_t* out_count) {
    *out_count = 0;
    char path[64];
    mappa_path_file_aula(id_risorsa, path, sizeof(path));

    FILE* file = fopen(path, "rb");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    size_t count = file_size / sizeof(prenotazione_t);
    prenotazione_t* prenotazioni = NULL;
    if (count > 0) {
        prenotazioni = malloc(count * sizeof(prenotazione_t));
        if (prenotazioni != NULL) fread(prenotazioni, sizeof(prenotazione_t), count, file);
    }
    fclose(file);

    *out_count = (prenotazioni != NULL) ? count : 0;
    return prenotazioni;
}

// Riscrive per intero il file dell'aula con l'array aggiornato (usata da approvazione/rifiuto)
static bool salva_prenotazioni_aula(int id_risorsa, const prenotazione_t* prenotazioni, size_t count) {
    char path[64];
    mappa_path_file_aula(id_risorsa, path, sizeof(path));

    FILE* file = fopen(path, "wb");
    if (file == NULL) return false;

    bool esito = (count == 0) || (fwrite(prenotazioni, sizeof(prenotazione_t), count, file) == count);
    fclose(file);
    return esito;
}

// Due prenotazioni sono in conflitto se riguardano la stessa data e le fasce orarie si sovrappongono
static bool si_sovrappongono(const prenotazione_t* a, const prenotazione_t* b) {
    if (strcmp(a->data, b->data) != 0) return false;
    return !(strcmp(a->ora_fine, b->ora_inizio) <= 0 || strcmp(a->ora_inizio, b->ora_fine) >= 0);
}