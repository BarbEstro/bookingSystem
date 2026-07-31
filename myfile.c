#include "myfile.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

FILE* aprireFile(const char* nome_file, const char* mod) {
    FILE* file = fopen(nome_file, mod);

    return file; 
}

bool scritturaFile(FILE* file, const char* stringa) {
    if (file == NULL || stringa == NULL) return false;
    
    size_t len = strlen(stringa);
    size_t bytes_scritti = fwrite(stringa, sizeof(char), len, file);
    
    return (bytes_scritti == len);
}

bool leggereRigaFile(FILE* file, char* buffer, size_t dimensione_buffer) {
    if (file == NULL || buffer == NULL) return false;
    
    if (fgets(buffer, (int)dimensione_buffer, file) != NULL) {
        return true;
    }
    
    return false;
}

void chiusuraFile(FILE* file) {
    if (file != NULL) {
        fclose(file);
    }
}