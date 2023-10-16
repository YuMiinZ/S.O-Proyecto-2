/*Proyecto #1. Sistemas Operativos
 *Elaborado por Tomás Coto y Ericka Guo
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> 
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

//Command -p //Reajuste de campos libres
void defragmentArchive() {//
}
//---------------------------------------------------------------------------------------------------------------

/*
void llenarDatosPrevios(struct TarHeader headder[], char *filesToAdd){
    char *token = strtok(filesToAdd, " ");
    int contador = 0;
    while (token != NULL) {
        printf("Nombre de archivo a empacar: %s\n", token);        
        *headder[contador].filename = token;
        contador++;
        token = strtok(NULL, " "); 
    }
}*/


//Command -c
/*void createArchive(char *tarName, char *filesToAdd) {
    printf("\nNombre del tarName: %s\n", tarName);
    
    /*FILE *tarFile = fopen(tarName, "wb");
    struct stat informacion_archivo;*/
    
    /*if (strcmp(tarName,"")==0) {
        printf("\n\nNombre del tar es nulo\n");
        exit(1);
    } else {
        printf("\n\nNombre del tar es %s\n", tarName);
        struct TarFileInput headder[100];
        struct FreeBlock free_block[100];
        struct stat informacion_archivo;
        size_t bytesRead;

        long displacement = 7200;
        int fileCount  = 0;
        FILE *fp_tar = fopen(tarName, "wb");
        if (fp_tar == NULL) {
            perror("Error al abrir el archivo tar");
            free(filesToAdd);
            exit(1);
        }
        fseek(fp_tar, 7200, SEEK_SET);
        char *token = strtok(filesToAdd, ", ");
        while (token != NULL) {
            printf("Nombre de archivo a empacar: %s\n", token);
            strcpy(headder[fileCount].filename, token);
            FILE *fp_archivo = fopen(token, "rb");
            if (fp_archivo == NULL) {
                perror("No se pudo abrir el archivo de origen");
                fclose(fp_tar);
                free(filesToAdd);  // Libera la copia de la cadena
                exit(1);
            }
            
            stat(token, &informacion_archivo);
            printf("tamaño: %ld\n",(long)informacion_archivo.st_size);
            headder[fileCount].size = (long)informacion_archivo.st_size;
            headder[fileCount].start_byte = displacement;
            headder[fileCount].end_byte = displacement+(long)informacion_archivo.st_size;
            printf("Puntero Final Suposición: %ld\n", headder[fileCount].end_byte);

            char buffer[4096]; // Un búfer para leer y escribir datos
            size_t bytes_leidos;

            while ((bytes_leidos = fread(buffer, 1, sizeof(buffer), fp_archivo) > 0)) {
                fwrite(buffer, 1, bytes_leidos, fp_tar);
            }
            //fwrite(fileToAdd, );
            //fseek(fileToAdd, 0, SEEK_END);
            //printf("Puntero Final: %ld\n", ftell(fileToAdd));
            //headder[fileCount].end_byte = displacement;
            displacement = headder[fileCount].end_byte;
            printf("Proximo startbyte: %ld\n\n", displacement);
            fclose(fp_archivo);
            fileCount ++;
            token = strtok(NULL, " "); 
        }
        /*for(int i=0;i<100;i++){
            strcpy(headder[i].filename, "sorueba");
            headder[i].size = 1200;
            headder[i].start_byte = i;
            headder[i].end_byte = 100+i;
        }
        fseek(tarFile, 7200, SEEK_SET);
        fwrite(headder, sizeof(headder), 1, tarFile); //4800
        fwrite(free_block, sizeof(free_block), 1, tarFile);//7200

        printf("Posicion del puntero: %ld", ftell(tarFile));*/
        //fclose(tarName);
        /*printf("Posición del puntero: %ld\n", ftell(tarFile));
         //Comienzo del parseo y agregar los archivos consecutivamente
        */
                //llenarDatosPrevios(headder, filesToAdd);


        //printf("\nLista de nombres de los archivos: %s\n", filesToAdd);
    //}
    
    /*
    */


    /*if (stat(nombre_archivo, &informacion_archivo) == 0) {
        long tamaño_archivo = (long)informacion_archivo.st_size;
        printf("El tamaño del archivo(s) es %ld bytes.\n", tamaño_archivo);
    } else {
        perror("Error al obtener el tamaño del archivo");
        return 1;
    }*/
//}

void verificarComandos(int argc, char *argv[], bool *verbose, bool *create, bool *extract, bool *list, bool *delete, 
                                 bool *update, bool *append, bool *pack, bool *foundF) {

    /*if(strcmp(argv[1],"star")!=0){
        printf("\nDebe de proporcionar el comando star  al inicio para comenzar...\nFormato basico: %s star <comando> \n",argv[0]);
        exit(0);
    }*/
    char listaComandos[] = "-cxtduvfrpelt ";
    //struct TarFile listaArchivos[100];

   // struct TarHead listaArchivos[10
    

    /*printf("Cantidad de argumentos: %ld\n", strlen(argv[3]));
    if (argc < 3) { //Verificando la cantidad mínima de parámetros - debe mínimo ingresar el comando
        printf("Formato: %s <comando> \n", argv[0]);
        exit(1);
    }*/

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

    //falta hacer bool para los casos y saber qué se debe de ejecutar.

    //Asumiento que el caso es create
    //tarName = argv[n];
    
    //printf("prueba %s\n",argv[2]);
    
    
    /*
    printf("Argumento 3: %s\n", argv[3]);

    char *token = strtok(argv[3], " "); 

    while (token != NULL) {
        printf("Nombre de archivo: %s\n", token);
        token = strtok(NULL, " "); 
    }
    */

    printf("Último comando ingresado: %c\n", argv[1][strlen(argv[1])-1]);
    //printf("Verbose %d\n", verbose);
}
/*
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

    for (int i = 0; i < 100; i++) {
        headder[i].start_byte = -1;  
    }
    fseek(fp_tar, 0, SEEK_END);
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
        /*while (0 < (bytes_leidos = fread(buffer, 1, sizeof(buffer), fp_archivo))) {
            fwrite(buffer, 1, bytes_leidos, fp_tar);
        }
        long end_byte = ftell(fp_tar);
        
        fclose(fp_archivo);

        // Llena el encabezado con los datos del archivo
        strcpy(headder[fileCount].filename, archivo);
        headder[fileCount].size = end_byte - start_byte;
        headder[fileCount].start_byte = start_byte;
        headder[fileCount].end_byte = end_byte;

        fileCount++;
        token = strtok(NULL, ",");
    }

    // Escribe el encabezado al principio
    fseek(fp_tar, 0, SEEK_SET);
    fwrite(headder, sizeof(struct EntryFile), 100, fp_tar);
    // Escribe los bloques libres después del encabezado
    fwrite(free_block, sizeof(struct FreeBlock), 100, fp_tar);

    fclose(fp_tar);
    free(archivos_copy);
    printf("Archivos copiados al archivo tar exitosamente.\n");
}*/

void pruebaRead2() {
    struct EntryFile empResult[100]; 
    FILE *f = fopen("pruebaTar.tar", "rb");

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
    FILE *fp_tar = fopen("pruebaTar.tar", "wb");
    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo de destino");
        exit(1);
    }

    struct stat informacion_archivo;
    struct EntryFile header[100];
    struct FreeBlock free_block[100];

    for (int i = 0; i < 100; i++) {
        free_block[i].start_byte = 1;
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
    printf("Archivos copiados al archivo tar exitosamente.\n");
}

void pruebaRead() {
    struct EntryFile empResult[100]; 
    FILE *f = fopen("pruebaTar.tar", "rb");

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

void pruebaExtract(){
    FILE *inputFile = fopen("pruebaTar.tar", "rb");
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

void pruebaRead1(){
    const char *tarFile = "pruebaTar.tar";
    FILE *fp_tar = fopen(tarFile, "rb");

    if (fp_tar == NULL) {
        perror("No se pudo abrir el archivo tar");
        exit(1);
    }

    struct FreeBlock free_block[100]; // Supongamos que deseas leer 100 estructuras FreeBlock

    // Calcula la posición donde comienzan los datos de FreeBlock en el archivo
    long freeBlockStart = sizeof(struct EntryFile) * 100;
    fseek(fp_tar, freeBlockStart, SEEK_SET);

    // Lee los datos de FreeBlock desde el archivo
    fread(free_block, sizeof(struct FreeBlock), 100, fp_tar);

    fclose(fp_tar);

    // Ahora tienes los datos de FreeBlock en el arreglo free_block
    for (int i = 0; i < 100; i++) {
        printf("Free Block %d, StartByte: %ld, EndByte: %ld\n", i, free_block[i].start_byte, free_block[i].end_byte);
    }
}

int main(int argc, char* argv[]) {
    
    //FILE *fileToPack = fopen("prueba.txt", "rb");
   // fseek(fileToPack, 512, SEEK_SET);
    //fseek(fileToPack, 0, SEEK_END);
    //printf("tamaño: %ld",ftell(fileToPack));
    printf("Cantidad de argumentos: %d\n", argc);
    printf("Lista de archivos: %s\n", argv[0]);
    printf("Lista de archivos: %s\n", argv[1]);
    printf("Lista de archivos: %s\n", argv[2]);
    printf("Lista de archivos: %s\n", argv[3]);
    printf("Lista de archivos: %s\n\n", argv[4]);
    create(argv[2], argv[3]); //Este es el create que funciona
    pruebaRead(); //Este muestra todo lo que hay en struct FileEntry
    pruebaRead1();
    //pruebaRead2(); //Este es el que funciona para listar */ 

    pruebaExtract(); //Funciona para extraer
    // Supongamos que ya has creado el archivo tar (tarFile) y definido los encabezados (headder).


    /*bool create = false,extract = false, list = false, delete = false, update = false, append = false, pack = false;
    bool verbose = false, foundF = false; //True = 1 | False = 0

    verificarComandos(argc, argv, &verbose, &create, &extract, &list, &delete, &update, &append, &pack, &foundF);

    printf("Verbose %d\n", verbose);
    printf("Found f %d\n\n---------------------------------------------------------", foundF);
    createArchive(argv[2],argv[3]);*/
    /*if(foundF){

        createArchive(argv[3],argv[4]);
    }
    else{
       // createStandar(argv[4]);
    }*/
    
    
    /*if(argv[3] == ' '){
        printf("Falta lista de archivos");
    }*/
    /*char *token = strtok(argv[3], " "); 

    while (token != NULL) {
        printf("Nombre de archivo: %s\n", token);
        token = strtok(NULL, " "); 
    }*/
    /*
    
    struct TarFile tarFiles[100];
    char fileName[20];
    char entryFileList;

    
    */

    //printf("Argumento 3: %s\n", argv[3]);
    /*const char* nombre_archivo = "prueba.txt";
    struct stat informacion_archivo;
    stat(nombre_archivo, &informacion_archivo);
    printf("El tamaño del archivo(s) es %ld bytes.\n", (long)informacion_archivo.st_size);*/
    /*if (stat(nombre_archivo, &informacion_archivo) == 0) {
        long tamaño_archivo = (long)informacion_archivo.st_size;
        printf("El tamaño del archivo(s) es %ld bytes.\n", tamaño_archivo);
    } else {
        perror("Error al obtener el tamaño del archivo");
        return 1;
    }*/

    /*char cadena[] = "asg-xdhj";
    char caracter = argv[1][0];

    char *resultado = strchr(cadena, caracter);

    if (resultado != NULL) {
        printf("El carácter '%c' está presente en la cadena.\n", caracter);
    } else {
        printf("El carácter '%c' no está presente en la cadena.\n", caracter);
    }*/
    //printf("prueba %c",argv[1][1]);
   /*printf("prueba tamaño: %d\n",argc);
    const char* comando = argv[1];
    const char* archivoEmpacado = argv[2];
    for (int i = 1; i < argc; i++) {
        printf("argumento: %s\n",argv[i]);
    }*/

    /*if (strcmp(argv[1], "-f") == 0) {
            // La opción -f se encontró, verifica si hay un argumento adicional (el nombre del archivo)
        printf("encontre comando f %s holaaaa\n", comando);

    }*/

   

    exit(0);
}