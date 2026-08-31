#ifndef CALENDARIO_H
#define CALENDARIO_H

#include <stdbool.h>

void carica_calendario_all_avvio();
int get_aule_totali();
int annotazione_prenotazione(int aula, int ora_inizio, int ora_fine);
int cancellazione_prenotazione(int aula, int ora_inizio, int ora_fine);


#endif