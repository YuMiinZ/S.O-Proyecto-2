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

int findBlock(struct FreeBlock *blocks, long size, int i){
    int noblock;
    for (i = 0; i < 100; i++){
        if (blocks[i].end_byte - blocks[i].start_byte >= size){
            noblock = 0;
            break;
        }
        noblock = 1;
    }
    return noblock;
}

void copyHeader(struct FreeBlock *blocks, struct EntryFile *files, char *tarName){
    FILE *f = fopen(tarName, "r+");
    fread(files, sizeof(struct EntryFile)*100, 1, f);
    fread(blocks, sizeof(struct FreeBlock)*100, 1, f);
    fclose(f);
}

void pasteHeader(struct FreeBlock *blocks, struct EntryFile *files, char *tarName){
    FILE *f = fopen(tarName, "r+");
    fwrite(files, sizeof(struct EntryFile)*100, 1, f);
    fwrite(blocks, sizeof(struct FreeBlock)*100, 1, f);
    fclose(f);
}

//Command -u
void update() { //
}

//Command -r append
void addFile(char *tarName, char *newFile) {//
    struct EntryFile files[100];
    struct FreeBlock blocks[100];
    char file0[20];
    struct stat informacion_archivo;

    FILE *f = fopen(tarName, "r+");
    copyHeader(blocks, files, tarName);
    strcpy(file0, files[0].filename);

    FILE *nf = fopen(newFile, "r");
    stat(newFile, &informacion_archivo);
    
    char filebuffer[informacion_archivo.st_size];
    int i, j, lastfile; 
    int noblock = 1;

    // Revisa si cabe en algun hueco existente
    noblock = (findBlock(blocks, informacion_archivo.st_size, i));
    // Revisa en que poiscion se agrega en el header
    for (j = 0; j < 100; j++){
        if (files[j].start_byte == 0 && files[j].end_byte == 0){
            break;
        }
        else{
            lastfile = j;
        }
    }
    fread(filebuffer, informacion_archivo.st_size, 1, nf);
    // Revisa si se tiene que mandar al final
    if (noblock){ 
        files[j].start_byte = files[lastfile].end_byte;
    } else{
        files[j].start_byte = blocks[i].start_byte;
    }
    printf("INicio :%ld\n", files[j].start_byte);
    // Agrega data al Header
    files[j].end_byte = files[j].start_byte + (long)informacion_archivo.st_size;
    files[j].size = (long)informacion_archivo.st_size;
    strcpy(files[j].filename, newFile);
    // Resiva si todavia queda espacio
    if (blocks[i].end_byte - blocks[i].start_byte == informacion_archivo.st_size){
        blocks[i].start_byte = 0;
        blocks[i].end_byte = 0;
    }
    else{
        blocks[i].start_byte = files[j].end_byte;
        strcpy(files[0].filename, file0);
    }
    fseek(f, files[j].start_byte, SEEK_SET);
    fwrite(filebuffer, informacion_archivo.st_size, 1, f);
    fclose(nf);

    pasteHeader(blocks, files, tarName);
    fclose(f);
    
}

void numTotal(struct FreeBlock *Free, struct EntryFile *Files, int *nums){
    for (int i = 0; i < 100; i++){
        if (Free[i].start_byte != 0 && Free[i].end_byte != 0){
            nums[0]++;
        }
        if (Files[i].start_byte != 0 && Files[i].end_byte != 0){
            nums[1]++;
        }
    }
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
void defragmentArchive(char *tarName) {
    struct EntryFile files[100];
    struct FreeBlock blocks[100];
    int numblocks[2];
    int count = 0;

    FILE *f = fopen(tarName, "r+");
    copyHeader(blocks, files, tarName);

    numTotal(blocks, files, numblocks);

    while (numblocks[0] >= 0 && count <= numblocks[1]+1){
        for (int i = 0; i < 100; i++){
            if (blocks[i].end_byte == 0 && blocks[i].end_byte == 0){
                continue;
            }

            for (int j = 0; j < 100; j++){
                if (blocks[i].end_byte != files[j].start_byte){
                    continue;
                }  
                char file[files[j].size];

                fseek(f, files[j].start_byte, SEEK_SET);
                fread(file, files[j].size, 1, f);
                printf("Posicion inicial archivo: %ld, posicion final: %ld\n", files[j].start_byte, files[j].end_byte);

                fseek(f, blocks[i].start_byte, SEEK_SET);
                fwrite(file, files[j].size, 1, f);

                files[j].start_byte = blocks[i].start_byte;
                blocks[i].end_byte = files[j].end_byte;
                files[j].end_byte =  blocks[i].start_byte = ftell(f);
                printf("Nueva posicion del archivo: I: %ld F: %ld\n", files[j].start_byte, files[j].end_byte);
                if (concatenateBlocks(blocks)){
                    numblocks[0]--;
                }
                printf("Posicion del bloque: I:%ld, F: %ld\n", blocks[0].start_byte, blocks[0].end_byte);
            }
        }
        count++;
    }

    pasteHeader(blocks, files, tarName);
    ftruncate(fileno(f), blocks[0].start_byte);
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

void list(char *tarName) { //Muestra Header con datos (lista contenido)
    struct EntryFile empResult[100]; 
    FILE *f = fopen(tarName, "rb");

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
            printf("Error a la hora de leer el archivo\n");
            fclose(f);
            exit(1);
        }
    } else {
        printf("Error al abrir el archivo de lectura\n");
        exit(1);
    }
}

void create(char *tarName, char *filesToAdd) {
    FILE *fp_tar = fopen(tarName, "wb");
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
    char *token = strtok(filesToAdd, ",");
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
    pasteHeader(free_block, header, tarName);
    fclose(fp_tar);
    free(archivos_copy);

    printf("Archivos copiados al archivo tar exitosamente.\n");
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

void extract(char *tarName, char *fileName, long start_byte, long end_byte){
    printf("Nombre Tar: %s, Nombre nuevo archivo: %s, Start Extract: %ld, End Extract: %ld\n\n",
    tarName, fileName, start_byte, end_byte);
    FILE *inputFile = fopen(tarName, "rb");
    if (inputFile == NULL) {
        perror("No se pudo abrir el archivo de entrada");
        exit(1);
    }

    FILE *outputFile = fopen(fileName, "wb");
    if (outputFile == NULL) {
        perror("No se pudo crear el archivo de salida");
        fclose(inputFile);
        exit(1);
    }

    long startByte = start_byte;
    long endByte = end_byte;

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

    printf("Se extrajeron %ld bytes al archivo pruebaExtract.txt\n\n", bytesRead);
}

struct EntryFile *getListHeader(char *tarName) {
    struct EntryFile *resultCopy = malloc(sizeof(struct EntryFile) * 100);

    if (resultCopy == NULL) {
        perror("Error al asignar memoria para copiar los datos");
        exit(1);
    }

    struct EntryFile empResult[100];
    FILE *f = fopen(tarName, "rb");

    if (f != NULL) {
        size_t result = fread(empResult, sizeof(struct EntryFile), 100, f);

        if (result > 0) {
            fclose(f);
            int resultCount = 0;

            for (int i = 0; i < 100; i++) {
                if (empResult[i].start_byte != 0 || empResult[i].end_byte != 0 || empResult[i].size != 0 || empResult[i].filename[0] != '\0'){
                    resultCopy[resultCount] = empResult[i];
                    resultCount++;
                }
            }

            // Establece el último elemento del resultado para indicar el final.
            resultCopy[resultCount].filename[0] = '\0';

            return resultCopy;
        } else {
            printf("No se pudo leer el archivo\n");
            free(resultCopy); // Libera la memoria antes de salir
            fclose(f);
            exit(1);
        }
    } else {
        printf("No se logró abrir el archivo\n");
        free(resultCopy); // Libera la memoria antes de salir
        exit(1);
    }
}

void startExtract(char *tarName, char *filesToExtract){ //Extrae solo lo primero y los datos están quemados
    printf("Nombre del paquete a extraer: %s\n", tarName);
    printf("Lista de archivos: %s\n", filesToExtract);
    if(filesToExtract[0] == '\0'){
        printf("No hay lista de archivos especificado, se procederá a extraer todo.\n\n");
        struct EntryFile *result = getListHeader(tarName);
        char newFilename[30];
        for (int i = 0; result[i].filename[0] != '\0'; i++) {
            printf("Data from file %d, StartByte: %ld, EndByte: %ld, Size: %ld, Name: %s\n",
                i, result[i].start_byte, result[i].end_byte, result[i].size, result[i].filename);
            strcpy(newFilename, "extract_");
            strcat(newFilename, result[i].filename);
            extract(tarName, newFilename, result[i].start_byte, result[i].end_byte);
        }
        free(result);
    } else {
        printf("Se extraerá solo los archivos especificados: %s\n\n", filesToExtract);
        struct EntryFile *result = getListHeader(tarName);
        char newFilename[30];
        char *token = strtok(filesToExtract, ", ");
        while (token != NULL) {
            for (int i = 0; result[i].filename[0] != '\0'; i++) {
                if(strcmp(result[i].filename, token)==0){
                    printf("Encontré el archivo a extraer: \n");
                    printf("Data from file %d, StartByte: %ld, EndByte: %ld, Size: %ld, Name: %s\n\n",
                    i, result[i].start_byte, result[i].end_byte, result[i].size, result[i].filename);
                    strcpy(newFilename, "extract_");
                    strcat(newFilename, result[i].filename);
                    extract(tarName, newFilename, result[i].start_byte, result[i].end_byte);
                    break;
                }                
            }
            token = strtok(NULL, ", ");
        }
        free(result);
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

void delete(char *tarName, char *fileName) {
    FILE *tp_tar = fopen(tarName, "r+b");
    if (tp_tar == NULL) {
        perror("No se pudo abrir el archivo tar");
        exit(1);
    }

    struct EntryFile header[100];
    fread(header, sizeof(struct EntryFile), 100, tp_tar);

    struct FreeBlock freeBlock[100];
    fread(freeBlock, sizeof(struct FreeBlock), 100, tp_tar);

    //Busca el archivo a eliminar y devuelve su posición en el arreglo del header
    int found = -1;
    for (int i = 0; i < 100; i++) {
        if (header[i].filename[0] != '\0' && strcmp(header[i].filename, fileName) == 0) {
            found = i;
            break;
        }
    }

    if (found != -1) {
        long start_byte = header[found].start_byte;
        long end_byte = header[found].end_byte;

        //Actualizar el bloque vacío
        freeBlock[found].start_byte = start_byte;
        freeBlock[found].end_byte = end_byte;

        fseek(tp_tar, start_byte, SEEK_SET);

        //Escribir bytes vacíos para borrar el contenido del archivo
        char emptyBuffer[1024];
        for (long j = start_byte; j < end_byte; j += sizeof(emptyBuffer)) {
            size_t bytesToWrite = sizeof(emptyBuffer);
            if (j + bytesToWrite > end_byte) {
                bytesToWrite = end_byte - j;
            }
            fwrite(emptyBuffer, 1, bytesToWrite, tp_tar);
        }

        //Poniendo datos de archivo eliminado
        header[found].start_byte = 0;
        header[found].end_byte = 0;
        header[found].size = 0;
        header[found].filename[0] = '\0';

        //Escribir el encabezado actualizado
        fseek(tp_tar, 0, SEEK_SET);
        fwrite(header, sizeof(struct EntryFile), 100, tp_tar);

        //Actualizar el bloque vacío
        fseek(tp_tar, sizeof(struct EntryFile) * 100, SEEK_SET);
        fwrite(freeBlock, sizeof(struct FreeBlock), 100, tp_tar);

        fclose(tp_tar);
        printf("Archivo eliminado: %s\n\n", fileName);
    } else {
        printf("Archivo no encontrado: %s\n\n", fileName);
        fclose(tp_tar);
    }
}

int sortFreeblockStructure(const void *a, const void *b) {
    return ((struct FreeBlock*)a)->start_byte < ((struct FreeBlock*)b)->start_byte;
}

int main(int argc, char* argv[]) {
    printf("Cantidad de argumentos: %d\n", argc);
    printf("./a.out: %s\n", argv[0]);
    printf("Comando: %s\n", argv[1]);
    printf("Archivo de salida .star: %s\n", argv[2]);

    printf("Lista de archivos: %s\n\n", argv[3]);
    //Comandos a usar

    //Verificar comandos para saber que función activar.
    //verificarComandos(argc, argv, &verbose, &create, &extract, &list, &delete, &update, &append, &pack, &foundF); 
    //create(argv[2], argv[3]); //Este es el create que funciona
    //startExtract(argv[2], argv[3]); //Funciona para extraer 1 en específico y con datos quemados, esto lo hice de acuerdo
    //list(argv[2]); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */
    //delete(argv[2], argv[3]); //Elimina 1 archivo, actualiza el header y los bloques libres
    //defragmentArchive(argv[2]);
    list(argv[2]); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */
    addFile(argv[2], argv[3]);
    list(argv[2]); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */


    //pruebaRead(); //Este muestra todo lo que hay en struct FileEntry
    //pruebaRead1(); //Muestra todos los espacios tanto libre como no libre
    //pruebaFreeBlocks(); //Muestra todos los libres (con datos de cuáles son libres)
    

    
                    
    exit(0);
}
