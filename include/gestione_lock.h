#ifndef GESTIONE_LOCK_H
#define GESTIONE_LOCK_H

int acquisisci_lock(int tipo_lock, const char* file_lock);
void rilascia_lock(int lock_fd);

#endif