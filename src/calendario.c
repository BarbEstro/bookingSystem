#include <calendario.h>
#include <stdio.h>
#include <booking_system_struct.h>

static disponibilita_aula_t *calendario_in_ram = NULL;
static int num_aule_totali = 0;

void carica_calendario_all_avvio() {
    FILE *file = fopen("dati/calendario.bn", "rb");
    if (!file) {
        perror("Errore apertura calendario");
        return;
    }

    fseek(file, 0, SEEK_END);
    num_aule_totali = ftell(file) / sizeof(disponibilita_aula_t);
    rewind(file); // Riporta il puntatore all'inizio (byte 0)

    calendario_in_ram = malloc(num_aule_totali * sizeof(disponibilita_aula_t));
    
    fread(calendario_in_ram, sizeof(disponibilita_aula_t), num_aule_totali, file);

    fclose(file);
    printf("[SERVER] Caricate %d aule direttamente in memoria!\n", num_aule_totali);
}
