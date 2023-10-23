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

// Ordena todos los EntryFiles de menor a mayor
int sortEntryFileStructure(const void *a, const void *b) {
    return ((struct EntryFile*)a)->start_byte < ((struct EntryFile*)b)->start_byte;
}

// Funcion para copiar la informacion del archivo al los struct correspondiente
void copyHeader(struct FreeBlock *blocks, struct EntryFile *files, char *tarName){
    FILE *f = fopen(tarName, "r+b");
    fread(files, sizeof(struct EntryFile)*100, 1, f);
    fread(blocks, sizeof(struct FreeBlock)*100, 1, f);
    fclose(f);
}

// Funcion para pegar el header en el archivo
void pasteHeader(struct FreeBlock *blocks, struct EntryFile *files, char *tarName){
    FILE *f = fopen(tarName, "r+b");
    fwrite(files, sizeof(struct EntryFile)*100, 1, f);
    fwrite(blocks, sizeof(struct FreeBlock)*100, 1, f);
    fclose(f);
}

void copySortFiles(struct EntryFile *filesO, struct EntryFile *tmpfiles){
    for (int i = 0; i < 100; i++){
        tmpfiles[i] = filesO[i];
    }
    qsort(tmpfiles, 100, sizeof(struct EntryFile), sortEntryFileStructure);
}

// Revisa el numero total de bloques y archivos
// lo que hace es contar como archivo y bloque si sus bytes son diferentes a 0
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

// funcion para concatenar bloques
// lo que hace es que revisa si un bloque en especifico se puede concatenar
bool concatenateBlocks(struct FreeBlock *Free, int i){
    for (int j = 0; j < 100; j++){
        if (Free[i].end_byte == Free[j].start_byte){
            Free[i].end_byte = Free[j].end_byte;
            Free[j].start_byte = 0;
            Free[j].end_byte = 0;
            return true;
        } 
        if (Free[i].start_byte == Free[j].end_byte){
            //Free[i].end_byte = Free[j].end_byte;
            Free[j].start_byte = 0;
            Free[j].end_byte = 0;
            return true;
        }
    }
    return false;
}

void concatenateBlocksDelete(struct FreeBlock *Free){
    for (int z = 0; z < 2; z++){
        for (int i = 0; i < 100; i++){
            if(Free[i].end_byte==0 && Free[i].start_byte==0){
                continue;
            }
            for(int j = 0; j < 100 ; j++){
                if (Free[i].end_byte == Free[j].start_byte){
                    Free[i].end_byte = Free[j].end_byte;
                    Free[j].start_byte = 0;
                    Free[j].end_byte = 0;
                    break;
                } 
                if (Free[i].start_byte == Free[j].end_byte){
                    Free[j].start_byte = 0;
                    Free[j].end_byte = 0;
                    break;
                }
            }
            break;
        }
    }
}

// Funcion para descubrir si un archivo entra en un hueco
// lo que hace es comparar el tamaño del archivo con el tamaño del hueco
int findSuitableBlock(struct FreeBlock *blocks, long size, int *i){
    int noblock;
    for (i[0] = 0; i[0] < 100; i[0]++){
        if (blocks[i[0]].end_byte - blocks[i[0]].start_byte >= size){
            noblock = 0;
            break;
        }
        noblock = 1;
    }
    return noblock;
}

// Funcion para revisar si donde guardar el espacio vacio
int findEmptyBlock(struct FreeBlock *blocks){
    int i;
    for (i = 0; i < 100; i++){
        if (blocks[i].start_byte == 0 && blocks[i].end_byte == 0){
            return i;
        }
    } 
    return -1;
}

// Funcion para revisar si un archivo existe
// lo que hace es recorrer el array hasta que llega donde los archivos se llamen igual
int findFile(struct EntryFile *files, char *Filename){
    int i = 0;
    for (i = 0; i < 100; i++){
        if (strcmp(Filename, files[i].filename) == 0){
            return i;
        }
    }
    
    return -1;
}

// Funcion para obtener el primer bloque libre que haya
// lo que hace es revisar el primer hueco con info que haya
int lastActualyBlock(struct FreeBlock *blocks){
    int j;
    for (j = 0; j < 100; j++){
        if (blocks[j].start_byte != 0 && blocks[j].end_byte != 0){
            return j;
        }
    }
    return -1;
}

// Funcion para ver si queda espacio en el bloque libre o se elimina
// lo que hace es revisar si el tamaño del archivo es el mismo que el del bloque
void space(struct FreeBlock *blocks, struct EntryFile *files, int indexBlock, int indexFile, long size, char *file0){
    if (blocks[indexBlock].end_byte - blocks[indexBlock].start_byte == size){
        blocks[indexBlock].start_byte = 0;
        blocks[indexBlock].end_byte = 0;
    }
    else{
        blocks[indexBlock].start_byte = files[indexFile].end_byte;
        strcpy(files[0].filename, file0);
    }
}
//---------------------------------------------------------------------------------------------------------------------



//Command -u
void startUpdateV(char *tarName, char *fileName) {
    struct EntryFile files[100];
    struct FreeBlock blocks[100];
    struct EntryFile tmpfiles[100];
    struct stat informacion_archivo;

    FILE *f = fopen(tarName, "r+b");
    copyHeader(blocks, files, tarName);
    copySortFiles(files, tmpfiles);

    int indexFile = findFile(files, fileName);
    if (indexFile == -1){
        printf("No existe el archivo\n");
        return;
    }
    int indexBlock = findEmptyBlock(blocks);
    int block[1];

    char fileBufferEmpty[files[indexFile].size];
    char file0[20];
    strcpy(file0, files[0].filename);

    FILE *nf = fopen(fileName, "rb");
    stat(fileName, &informacion_archivo);
    char *fileBuffer = (char *)malloc(informacion_archivo.st_size);
    fread(fileBuffer, informacion_archivo.st_size, 1, nf);
    fclose(nf);
    printf("Se copia la info del nuevo archivo\n");

    blocks[indexBlock].start_byte = files[indexFile].start_byte;
    blocks[indexBlock].end_byte = files[indexFile].end_byte;
    printf("Nuevo bloque disponible: Inicio: %ld, Final: %ld\n", 
            blocks[indexBlock].start_byte, blocks[indexBlock].end_byte);
    concatenateBlocks(blocks, indexBlock);
    
    //Se borra la informacion del star pero no del header
    fseek(f, files[indexFile].start_byte, SEEK_SET);
    fwrite(fileBufferEmpty, files[indexFile].size, 1, f);
    printf("se borra la informacion del archivo: %s, Inicia: %ld, Termina: %ld\n", 
            files[indexFile].filename, files[indexFile].start_byte, files[indexFile].end_byte);

    // Revisa si tiene que mandar la informacion al final
    if(findSuitableBlock(blocks, informacion_archivo.st_size, block) == 0){
        printf("se encuentra bloque donde poder guardarlo Inicia: %ld, Termina: %ld\n", 
                blocks[block[0]].start_byte, blocks[block[0]].end_byte);
        files[indexFile].start_byte = blocks[block[0]].start_byte;
    } else{
        printf("Ningun espacio vacio disponible por lo que se envia al final del documento, en el byte: %ld\n", 
                tmpfiles[0].end_byte);
        files[indexFile].start_byte = tmpfiles[0].end_byte;
    }
    // Pone el nuevo archivo en el star
    fseek(f, files[indexFile].start_byte, SEEK_SET);
    fwrite(fileBuffer, informacion_archivo.st_size, 1, f);
    files[indexFile].end_byte = ftell(f);
    space(blocks, files, indexBlock, indexFile, informacion_archivo.st_size, file0);
    printf("Archivo Actualizado Nombre: %s, Inicio: %ld, Final: %ld\n", 
            files[indexFile].filename, files[indexFile].start_byte, files[indexFile].end_byte);

    fclose(f);
    pasteHeader(blocks, files, tarName);

}
void startUpdate(char *tarName, char *fileName) {
    struct EntryFile files[100];
    struct FreeBlock blocks[100];
    struct EntryFile tmpfiles[100];
    struct stat informacion_archivo;

    FILE *f = fopen(tarName, "r+b");
    copyHeader(blocks, files, tarName);
    copySortFiles(files, tmpfiles);

    int indexFile = findFile(files, fileName);
    if (indexFile == -1){
        return;
    }
    int indexBlock = findEmptyBlock(blocks);
    int block[1];

    char fileBufferEmpty[files[indexFile].size];
    char file0[20];
    strcpy(file0, files[0].filename);

    FILE *nf = fopen(fileName, "rb");
    stat(fileName, &informacion_archivo);
    char *fileBuffer = (char *)malloc(informacion_archivo.st_size);
    fread(fileBuffer, informacion_archivo.st_size, 1, nf);
    fclose(nf);

    blocks[indexBlock].start_byte = files[indexFile].start_byte;
    blocks[indexBlock].end_byte = files[indexFile].end_byte;
    concatenateBlocks(blocks, indexBlock);
    
    //Se borra la informacion del star pero no del header
    fseek(f, files[indexFile].start_byte, SEEK_SET);
    fwrite(fileBufferEmpty, files[indexFile].size, 1, f);

    // Revisa si tiene que mandar la informacion al final
    if(findSuitableBlock(blocks, informacion_archivo.st_size, block) == 0){
        files[indexFile].start_byte = blocks[block[0]].start_byte;
    } else{
        files[indexFile].start_byte = tmpfiles[0].end_byte;
    }
    // Pone el nuevo archivo en el star
    fseek(f, files[indexFile].start_byte, SEEK_SET);
    fwrite(fileBuffer, informacion_archivo.st_size, 1, f);
    files[indexFile].end_byte = ftell(f);
    space(blocks, files, indexBlock, indexFile, informacion_archivo.st_size, file0);

    fclose(f);
    pasteHeader(blocks, files, tarName);
}

//Command -r append
void startAddFileV(char *tarName, char *newFilename) {//
    struct EntryFile files[100];
    struct EntryFile tmpfiles[100];
    struct FreeBlock blocks[100];
    char file0[20];
    struct stat informacion_archivo;

    copyHeader(blocks, files, tarName);
    copySortFiles(files, tmpfiles);
    FILE *nf = fopen(newFilename, "rb");
    stat(newFilename, &informacion_archivo);
    
    char fileBuffer[informacion_archivo.st_size];
    int j; 
    int noblock = 1;
    int indexBlock[1];

    // Revisa si cabe en algun hueco existente
    noblock = findSuitableBlock(blocks, informacion_archivo.st_size, indexBlock);
    // Revisa en que poiscion se agrega en el header
    for (j = 0; j < 100; j++){
        if (files[j].start_byte == 0 && files[j].end_byte == 0){
            break;
        }
    }
    
    // Copia la informacion del archivo a agregar
    fread(fileBuffer, informacion_archivo.st_size, 1, nf);
    printf("Se copia la información del nuevo archivo\n");
    fclose(nf);
    // Revisa si se tiene que mandar al final
    if (noblock == 1){ 
        printf("No se encuentra espacios disponibles para el archivo por lo que se almacena al final\n");
        files[j].start_byte = tmpfiles[0].end_byte;
    } else{
        printf("Se encuentra espacio disponible: Inicia: %ld, Termina: %ld\n", 
                blocks[indexBlock[0]].start_byte, blocks[indexBlock[0]].end_byte);
        files[j].start_byte = blocks[indexBlock[0]].start_byte;
    }

    // Agrega data al Header
    files[j].end_byte = files[j].start_byte + (long)informacion_archivo.st_size;
    files[j].size = (long)informacion_archivo.st_size;
    strcpy(files[j].filename, newFilename);
    printf("Se actualiza el header con: Indice: %d, Nombre: %s, Empieza: %ld, Termina: %ld\n",
            j, files[j].filename, files[j].start_byte, files[j].end_byte);
    
    // Revisa si todavia queda espacio
    strcpy(file0, files[0].filename);
    space(blocks, files, indexBlock[0], j, informacion_archivo.st_size, file0);

    printf("Nombre: %s\n", files[j].filename);
    // pega la informacion del archivo a agregar
    FILE *f = fopen(tarName, "r+b");
    fseek(f, files[j].start_byte, SEEK_SET);
    fwrite(fileBuffer, informacion_archivo.st_size, 1, f);
    printf("Se actualiza el .Star\n");
    fclose(f);
    pasteHeader(blocks, files, tarName);
    
}

void startAddFile(char *tarName, char *newFilename) {//
    struct EntryFile files[100];
    struct EntryFile tmpfiles[100];
    struct FreeBlock blocks[100];
    char file0[20];
    struct stat informacion_archivo;

    copyHeader(blocks, files, tarName);
    copySortFiles(files, tmpfiles);
    FILE *nf = fopen(newFilename, "rb");
    stat(newFilename, &informacion_archivo);
    
    char fileBuffer[informacion_archivo.st_size];
    int j; 
    int noblock = 1;
    int indexBlock[1];

    // Revisa si cabe en algun hueco existente
    noblock = findSuitableBlock(blocks, informacion_archivo.st_size, indexBlock);
    // Revisa en que poiscion se agrega en el header
    for (j = 0; j < 100; j++){
        if (files[j].start_byte == 0 && files[j].end_byte == 0){
            break;
        }
    }
    
    // Copia la informacion del archivo a agregar
    fread(fileBuffer, informacion_archivo.st_size, 1, nf);
    fclose(nf);
    // Revisa si se tiene que mandar al final
    if (noblock == 1){ 
        files[j].start_byte = tmpfiles[0].end_byte;
    } else{
        files[j].start_byte = blocks[indexBlock[0]].start_byte;
    }

    // Agrega data al Header
    files[j].end_byte = files[j].start_byte + (long)informacion_archivo.st_size;
    files[j].size = (long)informacion_archivo.st_size;
    strcpy(files[j].filename, newFilename);
    
    // Revisa si todavia queda espacio
    strcpy(file0, files[0].filename);
    space(blocks, files, indexBlock[0], j, informacion_archivo.st_size, file0);

    // pega la informacion del archivo a agregar
    FILE *f = fopen(tarName, "r+b");
    fseek(f, files[j].start_byte, SEEK_SET);
    fwrite(fileBuffer, informacion_archivo.st_size, 1, f);
    fclose(f);
    pasteHeader(blocks, files, tarName);
}

//Command -p //Reajuste de campos libres
void startDefragmentFileV(char *tarName) {
    struct EntryFile files[100];
    struct FreeBlock blocks[100];
    int numblocks[2];
    int count = 0;

    FILE *f = fopen(tarName, "r+b");
    copyHeader(blocks, files, tarName);

    numTotal(blocks, files, numblocks);
    printf("ES: %d\n", numblocks[0]);
    if (numblocks[0] == 0){
        printf("No hay espacios vacios para desfragmentar\n");
        return;
    }

    while (numblocks[0] >= 0 && count <= numblocks[1]+1){
        for (int i = 0; i < 100; i++){
            if (blocks[i].end_byte == 0 && blocks[i].end_byte == 0){
                continue;
            }

            for (int j = 0; j < 100; j++){
                if (blocks[i].end_byte != files[j].start_byte){
                    continue;
                }  
                printf("Se encuentra archivo que le siga al hueco\n");
                char file[files[j].size];

                fseek(f, files[j].start_byte, SEEK_SET);
                fread(file, files[j].size, 1, f);
                printf("Archivo: Nombre: %s Posicion inicial archivo: %ld, posicion final: %ld\n", 
                        files[j].filename, files[j].start_byte, files[j].end_byte);

                fseek(f, blocks[i].start_byte, SEEK_SET);
                fwrite(file, files[j].size, 1, f);

                files[j].start_byte = blocks[i].start_byte;
                blocks[i].end_byte = files[j].end_byte;
                files[j].end_byte =  blocks[i].start_byte = ftell(f);
                printf("Nueva posicion del archivo: I: %ld F: %ld\n", files[j].start_byte, files[j].end_byte);
                if (concatenateBlocks(blocks, i)){
                    printf("se encuentra otro hueco por lo que se fucionan\n");
                    numblocks[0]--;
                }
                printf("Posicion del bloque: I:%ld, F: %ld\n\n", blocks[i].start_byte, blocks[i].end_byte);
            }
        }
        count++;
    }
    int indexB = lastActualyBlock(blocks);
    ftruncate(fileno(f), blocks[indexB].start_byte);
    printf("Nuevo peso del archivo: %ld\n", blocks[indexB].start_byte);
    fclose(f);
    blocks[indexB].start_byte = 0;
    blocks[indexB].end_byte = 0;
    pasteHeader(blocks, files, tarName);

}

void startDefragmentFile(char *tarName) {
    struct EntryFile files[100];
    struct FreeBlock blocks[100];
    int numblocks[2];
    int count = 0;

    FILE *f = fopen(tarName, "r+b");
    copyHeader(blocks, files, tarName);

    numTotal(blocks, files, numblocks);
    printf("ES: %d\n", numblocks[0]);
    if (numblocks[0] == 0){
        printf("No hay espacios vacios para desfragmentar\n");
        return;
    }

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

                fseek(f, blocks[i].start_byte, SEEK_SET);
                fwrite(file, files[j].size, 1, f);

                files[j].start_byte = blocks[i].start_byte;
                blocks[i].end_byte = files[j].end_byte;
                files[j].end_byte =  blocks[i].start_byte = ftell(f);
                if (concatenateBlocks(blocks, i)){
                    numblocks[0]--;
                }
            }
        }
        count++;
    }
    int indexB = lastActualyBlock(blocks);
    ftruncate(fileno(f), blocks[indexB].start_byte);
    fclose(f);
    blocks[indexB].start_byte = 0;
    blocks[indexB].end_byte = 0;
    pasteHeader(blocks, files, tarName);
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
                *foundF = true; 
                break;
            }
            
        }
    }
}

//Command -t 
//Lista todo el contenido de los archivos registrados en el header de x star
void startList(char *tarName) { 
    struct EntryFile empResult[100];
    FILE *f = fopen(tarName, "rb"); //Abre el archivo

    if (f != NULL) {
        size_t result = fread(empResult, sizeof(struct EntryFile), 100, f); //Obtiene todos los datos

        if (result > 0) {
            fclose(f);

            for (int i = 0; i < result; i++) {
                if (empResult[i].start_byte != 0 || empResult[i].end_byte != 0 || empResult[i].size != 0 || empResult[i].filename[0] != '\0') {
                     //Imprime el contenido obtenido con sus respectivos datos
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

//Command -c
//Crea un nuevo star con sus respectivos archivos empaquetados
void startCreateV(char *tarName, char *filesToAdd) {
    printf("Nombre del star: %s, Archivos a empaquetar: %s\n\n", tarName, filesToAdd);
    FILE *fp_tar = fopen(tarName, "wb");
    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo de destino");
        exit(1);
    }

    //Creación de las estructuras
    //El header está conformado por EntryFiles y FreeBlock
    struct stat informacion_archivo;
    struct EntryFile header[100]; //Se registrarán todos los archivos empaquetados en el star
    struct FreeBlock free_block[100]; //Para el manejo de los espacios libres en el star

    //Llena los espacios libres con 0, indicando que no hay ninguno al crear el star, debido a que sus archivos de adjuntan
    //de manera secuencial
    for (int i = 0; i < 100; i++) {
        free_block[i].start_byte = 0;
        free_block[i].end_byte = 0;
    }

    char *archivo;
    int fileCount = 0;
    long contentStart = (sizeof(struct EntryFile) * 100) + (sizeof(struct FreeBlock) * 100); //Cálculo para saber dónde termina el header
    printf("Posición de inicio para el primer archivo a empaquetar: %ld\n",contentStart);

    fseek(fp_tar, contentStart, SEEK_END);
    char *token = strtok(filesToAdd, ",");
    while (token != NULL) {
        if (fileCount >= 100) {
            printf("Se alcanzó el límite de 100 archivos. No se copiarán más.\n");
            break;
        }

        //Inicio del proceso de empaquetar archivos, también se obtiene su respectiva información para añadirlo al header.
        printf("Nombre de archivo a empacar: %s\n", token);
        strcpy(header[fileCount].filename, token);
        FILE *fp_archivo = fopen(token, "rb");
        if (fp_archivo == NULL) {
            perror("No se pudo abrir el archivo de origen");
            fclose(fp_tar);
            exit(1);
        }
        
        stat(token, &informacion_archivo);
        printf("Tamaño del archivo: %ld\n",(long)informacion_archivo.st_size);
        header[fileCount].size = (long)informacion_archivo.st_size;
        header[fileCount].start_byte = contentStart;
        header[fileCount].end_byte = contentStart + (long)informacion_archivo.st_size;
        

        fseek(fp_tar, contentStart, SEEK_SET);
        printf("Posición de inicio para el archivo a empaquetar: %ld\n",contentStart);

        printf("Inicio de lectura: %ld\n", ftell(fp_tar));
        char buffer[4096]; 
        size_t bytes_leidos;

        //Empaquetando el archivo...
        while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), fp_archivo)) > 0) {
            fwrite(buffer, 1, bytes_leidos, fp_tar);
        }

        printf("Posición final: %ld\n", header[fileCount].end_byte);

        //Posicionando para saber en qué posición debe de comenzar a empaquetar el siguiente archivo
        contentStart = header[fileCount].end_byte;
        printf("Proxima posición de inicio para el siguiente archivo: %ld\n\n", contentStart);

        fclose(fp_archivo);
        fileCount++;
        token = strtok(NULL, ", ");
    }
    //Luego de finalizar con todos los archivos y la recolección de ssu respectiva información se inserta todo en el header
    fseek(fp_tar, 0, SEEK_SET);
    fwrite(header, sizeof(struct EntryFile), fileCount, fp_tar);

    fseek(fp_tar, sizeof(struct EntryFile) * 100, SEEK_SET);
    fwrite(free_block, sizeof(struct FreeBlock), 100, fp_tar);
    fclose(fp_tar);

    printf("Archivos copiados al archivo tar exitosamente.\n");
}

void pruebaRead(char *starName) { //Muestra todo el header
    struct EntryFile empResult[100]; 
    FILE *f = fopen(starName, "rb");

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

//Command -x
//Extrae un archivo en específico (esta no es la función principal, esta comienza a partir de la funcion startExtract)
void extractV(char *tarName, char *fileName, long start_byte, long end_byte){
    printf("Nombre star: %s, Nombre del archivo a extraer: %s, Posición de inicio del archivo: %ld, Posición final del archivo: %ld\n\n",
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

    printf("Extrayendo el archivo... \n");
    //Extrae el archivo
    while (startByte < endByte) {
        size_t bytesToRead = (endByte - startByte) < sizeof(buffer) ? (size_t)(endByte - startByte) : sizeof(buffer);
        size_t bytesReadThisIteration = fread(buffer, 1, bytesToRead, inputFile);

        fwrite(buffer, 1, bytesReadThisIteration, outputFile);

        startByte += bytesReadThisIteration;
        bytesRead += bytesReadThisIteration;
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("Se extrajeron %ld bytes. \n\n", bytesRead);
}

/*
    Obtiene todo el contenido de los archivos registrados en el header de x star y devuelve dicha estructura con sus respectivos
    datos.
*/
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
            free(resultCopy); 
            fclose(f);
            exit(1);
        }
    } else {
        printf("No se logró abrir el archivo\n");
        free(resultCopy); 
        exit(1);
    }
}

// Inicio de la función extract (command -x), donde verifica si debe de extraer archivos en específico o todos.
void startExtractV(char *tarName, char *filesToExtract){ //Extrae solo lo primero y los datos están quemados
    printf("Nombre del paquete a extraer: %s\n", tarName);
    printf("Lista de archivos: %s\n", filesToExtract);
    if(filesToExtract[0] == '\0'){
        printf("No hay lista de archivos especificado, se procederá a extraer todo.\n\n");
        struct EntryFile *result = getListHeader(tarName);
        char newFilename[30];
        for (int i = 0; result[i].filename[0] != '\0'; i++) {
            printf("Información del archivo %d, Posición de inicio: %ld, Posición final: %ld, Tamaño: %ld, Nombre: %s\n",
                i, result[i].start_byte, result[i].end_byte, result[i].size, result[i].filename);
            strcpy(newFilename, "extract_");
            strcat(newFilename, result[i].filename);
            extractV(tarName, newFilename, result[i].start_byte, result[i].end_byte);
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
                    printf("Información del archivo %d, Posición de inicio: %ld, Posición final: %ld, Tamaño: %ld, Nombre: %s\n\n",
                    i, result[i].start_byte, result[i].end_byte, result[i].size, result[i].filename);
                    strcpy(newFilename, "extract_");
                    strcat(newFilename, result[i].filename);
                    extractV(tarName, newFilename, result[i].start_byte, result[i].end_byte);
                    break;
                }                
            }
            token = strtok(NULL, ", ");
        }
        free(result);
    }
   
}

void pruebaRead1(char *tarName){ // Muestra todos los bloques libres libres y no libres
    const char *tarFile = tarName;
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

//Command -d / -delete 
//Elimina un archivo del star y actualiza el header, eliminándolo del EntryFiles y en el FreeBlock se le indica el tamaño de 
//Ese espacio nuevo libre
void startDeleteV(char *tarName, char *fileName) {
    printf("Nombre del archivo star: %s, nombre del archivo a eliminar: %s\n", tarName, fileName);
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
        printf("Se ha encontrado el archivo a eliminar. Se eliminará liberará el espacio desde la posición %ld hasta %ld\n",
        header[found].start_byte, header[found].end_byte);
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
        printf("Se ha escrito bytes vacíos para borrar el contenido del archivo\n");
        //Poniendo datos de archivo eliminado
        header[found].start_byte = 0;
        header[found].end_byte = 0;
        header[found].size = 0;
        header[found].filename[0] = '\0';

        //Escribir el encabezado actualizado
        fseek(tp_tar, 0, SEEK_SET);
        fwrite(header, sizeof(struct EntryFile), 100, tp_tar);

        //Actualizar el bloque vacío
        concatenateBlocksDelete(freeBlock);


        fseek(tp_tar, sizeof(struct EntryFile) * 100, SEEK_SET);
        fwrite(freeBlock, sizeof(struct FreeBlock), 100, tp_tar);

        printf("Se ha actualizado la lista de bloques vacíos y se ha eliminado el archivo de la lista de EntryFiles\n");

        fclose(tp_tar);
        printf("Archivo eliminado: %s\n\n", fileName);

    } else {
        printf("Archivo no encontrado: %s\n\n", fileName);
        fclose(tp_tar);
    }
}

int main(int argc, char* argv[]) {
    printf("Cantidad de argumentos: %d\n", argc);
    printf("./a.out: %s\n", argv[0]);
    printf("Comando: %s\n", argv[1]);
    printf("Archivo de salida .star: %s\n", argv[2]);

    printf("Lista de archivos: %s\n\n", argv[3]);

    //Declaración de variables booleanas a utilizar para saber qué comando ingresó el usuario para que se ejecute
    bool verbose = false, create = false, extract = false, list = false, delete = false, update = false, 
    append = false, pack = false, foundF = false;
    //Comandos a usar

    //Verificar comandos para saber que función activar.
    verificarComandos(argc, argv, &verbose, &create, &extract, &list, &delete, &update, &append, &pack, &foundF); 
    if(create && verbose){
        printf("Create con verbose\n");
        startCreateV(argv[2], argv[3]);
    } else if (create && !verbose ){
        printf("Create sin verbose\n");
    }

    else if(extract && verbose){
        printf("Extract con verbose\n");
        startExtractV(argv[2], argv[3]);
    } else if(extract && !verbose){
        printf("Extract sin verbose\n");
    }

    else if(list){ //List no necesita V debido a que solo muestra el contenido, su proceso es solo obtener la info.
        printf("list\n");
        startList(argv[2]);
    }

    else if(delete && verbose){
        printf("Delete con verbose\n");
        startDeleteV(argv[2], argv[3]);
    } else if(delete && !verbose){
        printf("Delete sin verbose\n");
    }

    else if(update && verbose){
        printf("update con verbose\n");
        startUpdateV(argv[2], argv[3]);
    } else if(update && !verbose){
        printf("update sin verbose\n");
        startUpdate(argv[2], argv[3]);
    }

    else if(append && verbose){
        printf("append con verbose\n");
        startAddFileV(argv[2], argv[3]);

    } else if(append && !verbose){
        printf("append sin verbose\n");
        startAddFile(argv[2], argv[3]);
    }

    else if(pack && verbose){
        printf("pack con verbose\n");
        startDefragmentFileV(argv[2]);
    } else if(pack && !verbose){
        printf("pack sin verbose\n");
        startDefragmentFile(argv[2]);
    }

    //create(argv[2], argv[3]); //Este es el create que funciona
    //startExtract(argv[2], argv[3]); //Funciona para extraer 1 en específico y con datos quemados, esto lo hice de acuerdo
    //list(argv[2]); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */
    //delete(argv[2], argv[3]); //Elimina 1 archivo, actualiza el header y los bloques libres
    //defragmentArchive(argv[2]);
    //list(argv[2]); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */
    //addFile(argv[2], argv[3]);
    //list(argv[2]); //Este es el que funciona para listar Muestra los archivos del header, tamaño, inicio, final, nombre */


    //pruebaRead(argv[2]); //Este muestra todo lo que hay en struct FileEntry
    //pruebaRead1(argv[2]); //Muestra todos los espacios tanto libre como no libre
    //pruebaFreeBlocks(); //Muestra todos los libres (con datos de cuáles son libres)
    
    
    //create(argv[2], argv[3]);
    //list(argv[2]); 

    //delete(argv[2], argv[3]);
    //defragmentArchive(argv[2]);
    //list(argv[2]); 
    //update(argv[2], argv[3]);
     
    //list(argv[2]);
    //addFile(argv[2], argv[3]);
    //list(argv[2]); 
    


    exit(0);
}
