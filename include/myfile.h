#ifndef MY_FILE_LIB_H
#define MY_FILE_LIB_H

#include <stdio.h>
#include <stdbool.h>


FILE* aprireFile(const char* nome_file, const char* mod);
bool scritturaFile(FILE* file, const char* stringa);
bool leggereRigaFile(FILE* file, char* buffer, size_t dimensione_buffer);
void chiusuraFile(FILE* file);

#endif 