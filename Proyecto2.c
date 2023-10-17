/*Proyecto #2. Sistemas Operativos
 *Elaborado por Tomás Coto y Ericka Guo
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

struct EntryFile {
    char filename[20];
    long size;
    long start_byte;
    long end_byte;
};

struct FreeBlock {
    long start_byte;
    long end_byte;
};




//Command -x
void extractArchive() {
}

//Command -t
void listContents() {
}

//Command --delete
void deleteFile() {
}

//Command -u
void update() { //
}

//Command -r append
void addFile() {//
}

int numBlock(struct FreeBlock *Free){
    int numFree = 0;
    for (int i = 0; i < 100; i++){
        if (Free[i].start_byte != 0 && Free[i].end_byte != 0){
            numFree++;
        }
    }
    return numFree; 
}

bool concatenateBlocks(struct FreeBlock *Free){
    for (int i = 0; i < 100; i++){
        if (Free[i].start_byte == 0 && Free[i].end_byte == 0){
            continue;
        }
        for (int j = 0; j < 100; j++){
            if (Free[i].end_byte == Free[j].start_byte){
                Free[i].end_byte = Free[j].end_byte;
                Free[j].start_byte = 0;
                Free[j].end_byte = 0;
                return true;
            }
        }
    }
    return false;
}

//Command -p //Reajuste de campos libres
void defragmentArchive() {
    struct EntryFile files[100];
    struct FreeBlock blocks[100];

    FILE *f = fopen("pruebaTar.star", "r+");
    fread(files, sizeof(struct EntryFile)*100, 1, f);
    fread(blocks, sizeof(struct FreeBlock)*100, 1, f);
    
    blocks[0].start_byte = 6400; // Se eliminan cuando delete se integre
    blocks[0].end_byte = 78085;  
    blocks[2].start_byte = 804707;
    blocks[2].end_byte = 6449256;

    int numRealBlocks = numBlock(blocks);
    while (numRealBlocks > 0){
        for (int i = 0; i < 100; i++){
            for (int j = 0; j < 100; j++){
                if (blocks[i].end_byte == files[j].start_byte){
                    char file[files[j].size];

                    fseek(f, files[j].start_byte, SEEK_SET);
                    fread(file, files[j].size, 1, f);

                    fseek(f, blocks[i].start_byte, SEEK_SET);
                    fwrite(file, files[j].size, 1, f);

                    files[j].start_byte = blocks[i].start_byte;
                    blocks[i].end_byte = files[j].end_byte;
                    files[j].end_byte =  blocks[i].start_byte = ftell(f);
                    //printf("fin: %ld\n", files[i].end_byte);
                    if (concatenateBlocks(blocks)){
                        numRealBlocks--;
                        if (numRealBlocks == 1){
                            numRealBlocks = 0;
                        }
                    }
                    break;
                }
            }
        }
    }
    fseek(f, 0, SEEK_SET);
    fwrite(files, sizeof(struct EntryFile)*100, 1, f);
    fwrite(blocks, sizeof(struct FreeBlock)*100, 1, f);
    ftruncate(fileno(f), blocks[0].end_byte);
    printf("NUM: %d\n", numRealBlocks);
    fclose(f);
}
//---------------------------------------------------------------------------------------------------------------
void verificarComandos(int argc, char *argv[], bool *verbose, bool *create, bool *extract, bool *list, bool *delete, 
                                 bool *update, bool *append, bool *pack, bool *foundF) {

    char listaComandos[] = "-cxtduvfrpelt ";

    for(int position=0; position<strlen(argv[1]);position++){
        if (strchr(listaComandos, argv[1][position]) == NULL) {
            printf("\nComando no válido: %c\n", argv[1][position]);
            exit(1);
        }
        else{
            if(argv[1][position] =='v'){
                *verbose = true; 
            }
            else if(argv[1][position] =='c'){
                *create = true; 
            }
            else if(argv[1][position] =='x'){
                *extract = true; 
            }
            else if(argv[1][position] =='t'){
                *list = true; 
            }
            else if(argv[1][position] =='d'){
                *delete = true; 
            }
            else if(argv[1][position] =='u'){
                *update = true; 
            }
            else if(argv[1][position] =='r'){
                *append = true; 
            }
            else if(argv[1][position] =='p'){
                *pack = true; 
            }
            else if(argv[1][position] =='f'){
                printf("Comando final encontrado: %c\n", argv[1][position]);
                *foundF = true; 
                break;
            }
            printf("Comando válido: %c\n", argv[1][position]);
            
        }
    }
    printf("Último comando ingresado: %c\n", argv[1][strlen(argv[1])-1]);
}

void pruebaRead2() { //Muestra Header con datos (lista contenido)
    struct EntryFile empResult[100]; 
    FILE *f = fopen("pruebaTar.star", "rb");

    if (f != NULL) {
        size_t result = fread(empResult, sizeof(struct EntryFile), 100, f);

        if (result > 0) {
            fclose(f);

            for (int i = 0; i < result; i++) {
                if (empResult[i].start_byte != 0 || empResult[i].end_byte != 0 || empResult[i].size != 0 || empResult[i].filename[0] != '\0') {
                    printf("Data from file %d, StartByte: %ld, EndByte: %ld, Size: %ld, Name: %s\n", i, 
                            empResult[i].start_byte, empResult[i].end_byte, empResult[i].size, empResult[i].filename);
                }
            }
        } else {
            printf("Error reading from the file\n");
            fclose(f);
            exit(1);
        }
    } else {
        printf("Error opening the file to read data\n");
        exit(1);
    }
}

void create(char *tarName, char *filesToAdd) {
    FILE *fp_tar = fopen("pruebaTar.star", "wb");
    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo de destino");
        exit(1);
    }

    struct stat informacion_archivo;
    struct EntryFile header[100];
    struct FreeBlock free_block[100];

    for (int i = 0; i < 100; i++) {
        free_block[i].start_byte = 0;
        free_block[i].end_byte = 0;
    }

    char *archivo;
    char *archivos_copy = strdup(filesToAdd);
    int fileCount = 0;
    long contentStart = (sizeof(struct EntryFile) * 100) + (sizeof(struct FreeBlock) * 100);
    
    fseek(fp_tar, contentStart, SEEK_END);
    char *token = strtok(filesToAdd, ", ");
    while (token != NULL) {
        if (fileCount >= 100) {
            printf("Se alcanzó el límite de 100 archivos. No se copiarán más.\n");
            break;
        }

        printf("Nombre de archivo a empacar: %s\n", token);
        strcpy(header[fileCount].filename, token);
        FILE *fp_archivo = fopen(token, "rb");
        if (fp_archivo == NULL) {
            perror("No se pudo abrir el archivo de origen");
            fclose(fp_tar);
            free(archivos_copy); 
            exit(1);
        }
        
        stat(token, &informacion_archivo);
        printf("tamaño: %ld\n",(long)informacion_archivo.st_size);
        header[fileCount].size = (long)informacion_archivo.st_size;
        header[fileCount].start_byte = contentStart;
        header[fileCount].end_byte = contentStart + (long)informacion_archivo.st_size;
        printf("Puntero Final Suposición: %ld\n", header[fileCount].end_byte);

        fseek(fp_tar, contentStart, SEEK_SET);
        printf("Inicio segun contentStart: %ld\n",contentStart);
        printf("Inicio de lectura: %ld\n", ftell(fp_tar));
        char buffer[4096]; 
        size_t bytes_leidos;

        while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), fp_archivo)) > 0) {
            fwrite(buffer, 1, bytes_leidos, fp_tar);
        }
        
        contentStart = header[fileCount].end_byte;
        printf("Proximo startbyte: %ld\n\n", contentStart);

        fclose(fp_archivo);
        fileCount++;
        token = strtok(NULL, ", ");
    }
    fseek(fp_tar, 0, SEEK_SET);
    fwrite(header, sizeof(struct EntryFile), fileCount, fp_tar);

    fseek(fp_tar, sizeof(struct EntryFile) * 100, SEEK_SET);
    fwrite(free_block, sizeof(struct FreeBlock), 100, fp_tar);
    
    printf("dios %d", fileCount);
    fclose(fp_tar);
    free(archivos_copy);

    printf("ASJDKAS %s,",header[1].filename);
    printf("Archivos copiados al archivo star exitosamente.\n");
}

void pruebaRead() { //Muestra todo el header
    struct EntryFile empResult[100]; 
    FILE *f = fopen("pruebaTar.star", "rb");

    if (f != NULL) {
        size_t result = fread(empResult, sizeof(struct EntryFile), 100, f);

        if (result > 0) {
            fclose(f);

            for (int i = 0; i < result; i++) {
                printf("Data from file %d, StartByte: %ld, EndByte: %ld, Size: %ld, Name: %s\n", i, 
                        empResult[i].start_byte, empResult[i].end_byte, empResult[i].size, empResult[i].filename);
            }
        } else {
            printf("Error reading from the file\n");
            fclose(f);
            exit(1);
        }
    } else {
        printf("Error opening the file to read data\n");
        exit(1);
    }
}

void pruebaExtract(){ //Extrae solo lo primero y los datos están quemados
    FILE *inputFile = fopen("pruebaTar.star", "rb");
    if (inputFile == NULL) {
        perror("No se pudo abrir el archivo de entrada");
        exit(1);
    }

    FILE *outputFile = fopen("pruebaExtract.txt", "wb");
    if (outputFile == NULL) {
        perror("No se pudo crear el archivo de salida");
        fclose(inputFile);
        exit(1);
    }

    long startByte = 733022;
    long endByte = 1459644;

    fseek(inputFile, startByte, SEEK_SET);

    char buffer[1024];
    long bytesRead = 0;

    while (startByte < endByte) {
        size_t bytesToRead = (endByte - startByte) < sizeof(buffer) ? (size_t)(endByte - startByte) : sizeof(buffer);
        size_t bytesReadThisIteration = fread(buffer, 1, bytesToRead, inputFile);

        fwrite(buffer, 1, bytesReadThisIteration, outputFile);

        startByte += bytesReadThisIteration;
        bytesRead += bytesReadThisIteration;
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("Se extrajeron %ld bytes al archivo pruebaExtract.txt\n", bytesRead);
}

void pruebaRead1_1(){ // Muestra los bloques libres (Ninguno tiene dato xq no hay ninguno libre)
    const char *tarFile = "pruebaTar.star";
    FILE *fp_tar = fopen(tarFile, "rb");

    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo star");
        exit(1);
    }

    struct FreeBlock free_block[100]; 

    long freeBlockStart = sizeof(struct EntryFile) * 100;
    fseek(fp_tar, freeBlockStart, SEEK_SET);

    fread(free_block, sizeof(struct FreeBlock), 100, fp_tar);

    fclose(fp_tar);

    for (int i = 0; i < 100; i++) {
        if (free_block[i].start_byte != 0 || free_block[i].end_byte != 0) {
            printf("Free Block %d, StartByte: %ld, EndByte: %ld\n", i, free_block[i].start_byte, free_block[i].end_byte);
        }
    }
}

void pruebaRead1(){ // Muestra todos los bloques libres libres y no libres
    const char *tarFile = "pruebaTar.star";
    FILE *fp_tar = fopen(tarFile, "rb");

    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo star");
        exit(1);
    }

    struct FreeBlock free_block[100]; 

    long freeBlockStart = sizeof(struct EntryFile) * 100;
    fseek(fp_tar, freeBlockStart, SEEK_SET);

    fread(free_block, sizeof(struct FreeBlock), 100, fp_tar);

    fclose(fp_tar);

    for (int i = 0; i < 100; i++) {
        printf("Free Block %d, StartByte: %ld, EndByte: %ld\n", i, free_block[i].start_byte, free_block[i].end_byte);
    }
}

int main(int argc, char* argv[]) {
    printf("Cantidad de argumentos: %d\n", argc);
    printf("Lista de archivos: %s\n", argv[0]);
    printf("Lista de archivos: %s\n", argv[1]);
    printf("Lista de archivos: %s\n", argv[2]);
    printf("Lista de archivos: %s\n", argv[3]);
    printf("Lista de archivos: %s\n\n", argv[4]);
    defragmentArchive();
    //Verificar comandos para saber que función activar.
    //verificarComandos(argc, argv, &verbose, &create, &extract, &list, &delete, &update, &append, &pack, &foundF); 
    //create(argv[2], argv[3]); //Este es el create que funciona
    pruebaRead(); //Este muestra todo lo que hay en struct FileEntry
    pruebaRead1(); //Muestra todos los espacios tanto libre como no libre
    //pruebaRead1_1(); //Muestra todos los libres (con datos de cuáles son libres)
    //pruebaRead2(); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */

    //pruebaExtract(); //Funciona para extraer 1 en específico y con datos quemados, esto lo hice de acuerdo
                    //Con los datos de pruebaRead2() para ver si me extraia bien los archivos
                    //Solo se testeó ingresar 2 veces prueba.txt
                    
    exit(0);
}
