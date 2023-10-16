#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct EntryFile {
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

void prueba2(char *tarName, char *filesToAdd) {
    FILE *fp_tar = fopen("pruebaTar.tar", "wb");
    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo de destino");
        exit(1);
    }

    struct stat informacion_archivo;
    struct EntryFile headder[100];
    struct FreeBlock free_block[100];

    char *archivo;
    char *token;
    char *archivos_copy = strdup(filesToAdd);
    int fileCount = 0;

    while (fileCount < 100 && (token = strtok(archivos_copy, ",")) != NULL) {
        archivo = token;
        FILE *fp_archivo = fopen(archivo, "rb");
        if (fp_archivo == NULL) {
            perror("No se pudo abrir el archivo de origen");
            fclose(fp_tar);
            free(archivos_copy);
            exit(1);
        }

        // Copiar los datos del archivo al archivo de destino
        char buffer[1024];
        size_t bytes_leidos;
        long start_byte = ftell(fp_tar);
        while (0 < (bytes_leidos = fread(buffer, 1, sizeof(buffer), fp_archivo)) {
            fwrite(buffer, 1, bytes_leidos, fp_tar);
        }
        long end_byte = ftell(fp_tar);

        fclose(fp_archivo);

        // Llenar el encabezado con los datos del archivo
        strcpy(headder[fileCount].filename, archivo);
        headder[fileCount].size = end_byte - start_byte;
        headder[fileCount].start_byte = start_byte;
        headder[fileCount].end_byte = end_byte;

        fileCount++;
        token = strtok(NULL, ",");
    }

    // Escribir el encabezado completo al principio
    fseek(fp_tar, 0, SEEK_SET);
    fwrite(headder, sizeof(struct EntryFile), fileCount, fp_tar);

    // Luego, incluimos los bloques libres en el archivo
    if (fileCount < 100) {
        fseek(fp_tar, fileCount * sizeof(struct EntryFile), SEEK_SET);
        fwrite(free_block, sizeof(struct FreeBlock), 100 - fileCount, fp_tar);
    }

    fclose(fp_tar);
    free(archivos_copy);
    printf("Archivos copiados al archivo tar exitosamente.\n");
}
