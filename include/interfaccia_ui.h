#ifndef INTERFACCIA_UI_H
#define INTERFACCIA_UI_H

#include "booking_system_struct.h"

void interfaccia_utente_cliente();
void interfaccia_utente_admin();
void interfaccia_login();

op_cliente_t operazioni_login();
op_cliente_t operazioni_cliente();
op_cliente_t operazioni_admin();


#endif // INTERFACCIA_UI_H