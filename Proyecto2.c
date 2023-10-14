/*Proyecto #1. Sistemas Operativos
 *Elaborado por Tomás Coto y Ericka Guo
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct FileEntry {
    char filename[20];
    long size;
    long start_byte;
    long end_byte;
};

struct FreeBlock {
    long start_byte;
    long end_byte;
    struct FreeBlock *next;
};


//Command -c
void createArchive() {
}

//Command -x
void extractArchive() {
}

//Command -t
void listContents() {
}

//Command --delete
void deleteFile() {
}

//Command -u //Reajuste de campos libres
void update() {
}

//Command -r append
void addFile() {
}

//Command -p //Reajuste de campos libres
void defragmentArchive() {
}



int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Formato: %s <comando> <archivo_empacado> [archivos...]\n", argv[0]);
        exit(0);
    }

    const char* comando = argv[1];
    const char* archivoEmpacado = argv[2];

    printf("holaaaa %s holaaaa", archivoEmpacado);

    exit(0);
}