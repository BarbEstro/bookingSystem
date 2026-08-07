#ifndef GESTIONE_OPERAZIONI_SERVER_H
#define GESTIONE_OPERAZIONI_SERVER_H

#include "booking_system_struct.h"

risposta_server_t operazione_login(richiesta_t richiesta);
risposta_server_t operazione_registrazione(richiesta_t richiesta);

#endif // GESTIONE_OPERAZIONI_SERVER_H