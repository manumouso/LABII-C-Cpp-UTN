// GestionLista.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define LEGAJO "\n\tLegajo: %d\n"
#define FECHA_NOTA "\t\tFecha:%0.2hhd/%0.2hhd/%d\tNota: %.2f\n"

const int MAX_RUTA = 100;
const int MAX_CARACTERES = 31;
const int MAX_TEL = 11;
const int MAX_LINEA = 200;

typedef struct Alumno {
	int legajo;
	char carrera;
	char nombre[MAX_CARACTERES];
	char apellido[MAX_CARACTERES];
	char email[MAX_CARACTERES];
	char telefono[MAX_TEL];
}STR_ALUMNO;

typedef struct Carrera {
	char codigo;
	char nombre[MAX_CARACTERES];
	char descripcion[MAX_CARACTERES];
}STR_CARRERA;

typedef struct Parcial {
    int legajo;
    int fecha;//AAAAMMDD
    float nota;
}STR_PARCIAL;

typedef struct Promedio {
    int legajo;
    float promedio;
}STR_PROMEDIO;

typedef struct Fecha {
    unsigned char dia;
    unsigned char mes;
    unsigned short year;
}STR_FECHA;

typedef struct Dato {
    int legajo;
    int pos;
}STR_DATO;

typedef struct Nodo {
    STR_DATO dato;
    Nodo* siguiente;
} STR_NODO;


FILE* abrir(const char* fileName, const char* modo);
void cerrar(FILE* file);
size_t leer(STR_PARCIAL* registro, FILE* file);
size_t escribir(STR_PARCIAL registro, FILE* file);
size_t readAt(FILE *file, STR_PARCIAL *registro, int posicion);
STR_FECHA obtenerFecha(int formatoFecha);
void tokenNull(const char* token, FILE* texto, FILE* binario);
void textoBinario(const char* rutaTexto, const char* rutaBinaria, const char* separadores);
void binarioTexto(const char* rutaTexto, const char* rutaBinaria);

void create(STR_NODO** lista);
bool isEmpty(STR_NODO* lista);
STR_NODO* insertInFront(STR_DATO valor, STR_NODO** lista);
STR_NODO* insertAtEnd(STR_DATO valor, STR_NODO** lista);
STR_NODO* insertOrdered(STR_DATO valor, STR_NODO** lista);
void print(STR_NODO* lista);
void printDato(STR_DATO dato);
void clearList(STR_NODO** lista);
STR_NODO* search(STR_NODO* lista, STR_DATO valor);
STR_DATO deleteFirst(STR_NODO** lista);
void sort(STR_NODO** lista);
int count(STR_NODO* listaAux);

//1)compilar sin warnings: Proyecto->Propiedades->C/C++->Preprocesador->Def. de Preprocesado-> _CRT_SECURE_NO_WARNINGS;<opciones...>
//Configuracion->Todas las configuraciones. Plataforma->X64
//2)argumento <materia>.dat: Depurar->Propiedades->Depuracion->ArgumentosDeComando-> <materia>.dat
int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("No ingresaste la ruta del archivo <materia>.dat");
        exit(EXIT_FAILURE);
    }
    char rutaPromedio[MAX_RUTA]="promedios";
    strcat(rutaPromedio, argv[1]);
    
    FILE* materia = abrir(argv[1], "rb");
    FILE* promedios = abrir(rutaPromedio, "wb");

    STR_PARCIAL unParcial,parcialActual;
    STR_FECHA fecha;
    STR_PROMEDIO alumno;
    STR_DATO dato;
    STR_NODO* lista;
    create(&lista);
    int pos = 0;
    int cantidad = 0;
    int i = 0;
     
    leer(&unParcial, materia);
    while (!feof(materia)) {
        dato.legajo = unParcial.legajo;
        dato.pos = pos;
        pos++;
        insertOrdered(dato, &lista);
        leer(&unParcial, materia);
    }
    cantidad = count(lista);

    if (!isEmpty(lista)) {
        dato = deleteFirst(&lista);
        readAt(materia, &unParcial, dato.pos);
        while (i<cantidad) {
            alumno.promedio = 0;
            parcialActual.legajo = unParcial.legajo;
            printf(LEGAJO, parcialActual.legajo);
            while (i<cantidad && parcialActual.legajo == dato.legajo) {
                fecha = obtenerFecha(unParcial.fecha);
                printf(FECHA_NOTA, fecha.dia, fecha.mes, fecha.year, unParcial.nota);
                alumno.promedio += unParcial.nota;
                alumno.legajo = unParcial.legajo;
                if (!isEmpty(lista)){ 
                    dato = deleteFirst(&lista);
                    readAt(materia, &unParcial, dato.pos);
                }
                i++;
            }
            alumno.promedio = alumno.promedio / 2;
            fwrite(&alumno, sizeof(STR_PROMEDIO), 1, promedios);
        }
    }
        
    cerrar(materia);
    cerrar(promedios);
    clearList(&lista);
   
    return 0;
}

void create(STR_NODO** lista) {
    *lista = NULL;
    return;
}

bool isEmpty(STR_NODO* lista) {
    return lista == NULL;
}

STR_NODO* insertInFront(STR_DATO valor, STR_NODO** lista) {
    STR_NODO* nodo = (STR_NODO*)malloc(sizeof(STR_NODO));
    if (nodo) {
        nodo->dato = valor;
        nodo->siguiente = *lista;

        *lista = nodo;
    }
   
    return nodo;
}

STR_NODO* insertAtEnd(STR_DATO valor, STR_NODO** lista) {
    STR_NODO* nodo = (STR_NODO*)malloc(sizeof(STR_NODO));
    if (nodo) {
        nodo->dato = valor;
        nodo->siguiente = NULL;

        if (isEmpty(*lista)) {
            *lista = nodo;
        }
        else {
            STR_NODO* listaAux = *lista;
            while (listaAux->siguiente != NULL) {
                listaAux = listaAux->siguiente;
            }
            listaAux->siguiente = nodo;
        }
    }
    
    return nodo;
}

STR_NODO* insertOrdered(STR_DATO valor, STR_NODO** lista) {
    STR_NODO* nodo = (STR_NODO*)malloc(sizeof(STR_NODO));
    if (nodo) {
        nodo->dato = valor;
        nodo->siguiente = NULL;

        STR_NODO* listaAux = *lista;
        STR_NODO* nodoAnt = NULL;
        while (listaAux != NULL && listaAux->dato.legajo < valor.legajo) {
            nodoAnt = listaAux;
            listaAux = listaAux->siguiente;
        }

        if (nodoAnt == NULL) {
            *lista = nodo;
        }
        else {
            nodoAnt->siguiente = nodo;
        }

        nodo->siguiente = listaAux;
    }
    
    return nodo;
}

void print(STR_NODO* lista) {
    STR_NODO* listaAux;
    listaAux = lista;
    printf("POSICION\t\tLEGAJO\n");
    while (listaAux != NULL) {
        printf("%d\t\t%d\n", listaAux->dato.pos, listaAux->dato.legajo);
        listaAux = listaAux->siguiente;
    }
}

void printDato(STR_DATO dato)
{
    printf("%d   ", dato.legajo);
    
}

void clearList(STR_NODO** lista) {
    STR_NODO* aux = NULL;
    while (*lista != NULL) {
        aux = *lista;
        *lista = (*lista)->siguiente;
        free(aux);
    }
}

STR_NODO* search(STR_NODO* lista, STR_DATO valor) {
    STR_NODO* listaAux = lista;
    while (listaAux != NULL && listaAux->dato.legajo != valor.legajo) {
        listaAux = listaAux->siguiente;
    }
    return listaAux;
}

STR_DATO deleteFirst(STR_NODO** lista) {
    STR_DATO dato = (*lista)->dato;
    STR_NODO* aux = *lista;
    *lista = (*lista)->siguiente;
    free(aux);
    return dato;
}

void sort(STR_NODO** lista) {
    STR_NODO* listaAux;
    create(&listaAux);
    STR_DATO dato;
    while (*lista != NULL) {
        dato = deleteFirst(lista);
        insertOrdered(dato, &listaAux);
    }

    *lista = listaAux;
}

int count(STR_NODO* listaAux) {
    int cant = 0;
    while (listaAux != NULL) {
        listaAux = listaAux->siguiente;
        cant++;
    }

    return cant;
}

FILE* abrir(const char* fileName, const char* modo)
{
    FILE* file = fopen(fileName, modo);
    if (file == NULL)
    {
        fprintf(stderr, "No se pudo abrir el archivo: %s", fileName);
        exit(EXIT_FAILURE);
    }

    return file;
}

void cerrar(FILE* file) {
    fclose(file);
    file = NULL;
}

size_t leer(STR_PARCIAL* registro, FILE* file) {
    return fread(registro, sizeof(STR_PARCIAL), 1, file);
}
size_t escribir(STR_PARCIAL registro, FILE* file) {
    return fwrite(&registro, sizeof(STR_PARCIAL), 1, file);
}

size_t readAt(FILE* file, STR_PARCIAL* registro, int posicion)
{
    fseek(file, sizeof(STR_PARCIAL) * posicion, SEEK_SET);
    return fread(registro, sizeof(STR_PARCIAL), 1, file);
}

STR_FECHA obtenerFecha(int formatoFecha) {
    STR_FECHA fecha;
    fecha.dia = (unsigned char)((formatoFecha % 10000) % 100);
    fecha.mes = (unsigned char)((formatoFecha % 10000) / 100);
    fecha.year = (unsigned short)(formatoFecha / 10000);
    return fecha;
}

void tokenNull(const char* token, FILE* texto, FILE* binario) {

	if (token == NULL) {
		printf("Fallo parseo.\n");
		cerrar(texto);
		cerrar(binario);
		exit(EXIT_FAILURE);
	}
	return;
}

void textoBinario(const char* rutaTexto,const char* rutaBinaria,const char* separadores){

	FILE* texto = fopen(rutaTexto, "r");
	if (!texto) {
		printf("No se pudo abrir el archivo de texto \n");
		return;
	}
	FILE* binario = fopen(rutaBinaria, "wb");
	if (!binario) {
		fclose(texto);
		printf("No se pudo crear el archivo binario \n");
		return ;
	}
	STR_PARCIAL unParcial;
	
	char linea[MAX_LINEA];
	char* token = NULL;
	
	while (fgets(linea, MAX_LINEA - 1, texto)!=NULL) {
		token = strtok(linea, separadores);
		tokenNull(token, texto, binario);
        unParcial.legajo = atoi(token);

		token = strtok(NULL, separadores);
		tokenNull(token, texto, binario);
        unParcial.fecha= atoi(token);

		token = strtok(NULL, separadores);
		tokenNull(token, texto, binario);
        unParcial.nota= (float)atof(token);

		fwrite(&unParcial, sizeof(STR_PARCIAL), 1, binario);
	}	
		
	fclose(texto);
	fclose(binario);
	texto = NULL;
	binario = NULL;
	if (token != NULL) {
		token = NULL;
	}
	return;
}

void binarioTexto(const char* rutaTexto,const char* rutaBinaria) {
	
	FILE* texto = fopen(rutaTexto, "w");
	if (!texto) {
		printf("No se pudo crear el archivo de texto \n");
		return;
	}
	FILE* binario = fopen(rutaBinaria, "rb");
	if (!binario) {
		fclose(texto);
		printf("No se pudo abrir el archivo binario \n");
		return;
	}

	STR_PARCIAL unParcial;
	fread(&unParcial, sizeof(STR_PARCIAL), 1, binario);
	while (!feof(binario)) {
		fprintf(texto,"%d\t%d\t%f", unParcial.legajo, unParcial.fecha, unParcial.nota);
		fread(&unParcial, sizeof(STR_PARCIAL), 1, binario);
	}
	fclose(texto);
	fclose(binario);
	texto=NULL;
	binario=NULL;
	return;
}
