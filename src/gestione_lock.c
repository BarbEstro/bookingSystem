#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include "gestione_lock.h"

int acquisisci_lock(int tipo_lock, const char* file_lock) {
    int fd = open(file_lock, O_CREAT | O_RDWR, 0666);
    if (fd < 0) return -1;
    if (flock(fd, tipo_lock) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

void rilascia_lock(int lock_fd) {
    if (lock_fd < 0) return;
    flock(lock_fd, LOCK_UN);
    close(lock_fd);
}