// ImplementacionColaVector.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

const int CAPACIDAD_COLA = 10;

typedef struct Cola {
    int tope;//cantidad de elementos del vector
    int principio;
    int fin;
    int elementos[CAPACIDAD_COLA] = {0};//vector donde se guardan los valores aleatorios
}STR_COLA;

void create(STR_COLA* cola);
bool isEmpty(STR_COLA cola);
void add(int valor, STR_COLA* cola);
int remove(STR_COLA* cola);
void print(STR_COLA* cola);

int main()
{
    srand(time(NULL));
    STR_COLA cola;
    create(&cola);
    int i = 0;
    char confirmar = 'n';

    printf("Desea agregar un valor aleatorio a la cola? [S/N]: [ ]\b\b");
    scanf(" %c", &confirmar);

    while ((confirmar == 'S' || confirmar == 's') && i < CAPACIDAD_COLA) {

        add(rand() % 1000/*i*/,&cola);
        if (cola.tope < CAPACIDAD_COLA) {
            printf("Desea agregar un valor aleatorio a la cola? [S/N]: [ ]\b\b");
            scanf(" %c", &confirmar);
        }
        else {
            printf("No puede agregar mas elementos a la cola\n");
            printf("Tope=capacidad de la cola= %d\n", cola.tope);
        }
        i++;
    }
    
    print(&cola);
    printf("\nCola vacia\n");
    return 0;
}

void create(STR_COLA* cola) {
    cola->tope = 0;
    cola->principio = 0;
    cola->fin = 0;
}

bool isEmpty(STR_COLA cola) {
    return cola.tope == 0;
}
//pre: recibe un entero aleatorio y el puntero a la cola 
//post: agrega un elemento al vector al final siempre que sea posible
void add(int valor, STR_COLA* cola) {
    if (cola->tope < CAPACIDAD_COLA) {
        cola->elementos[cola->fin] = valor;
        (cola->tope)++;
        (cola->fin)++;
    }
        
}
//pre: recibe el puntero a la cola 
//post: devuelve el elemento que se encuentre al principio de la cola
//mueve todos los otros elementos una pos hacia el principio, para respetar contrato FIFO
//y todos los elementos salgan de la cola por la pos 0, para que ademas queden todas las pos del vector
//disponibles para que ingresen nuevos elementos
int remove(STR_COLA* cola) {
    int valor = cola->elementos[cola->principio];
    
    for (int i = 0; i < cola->tope; i++) {
        cola->elementos[i]= cola->elementos[i+1];
    }
    
    (cola->fin)--;
    (cola->tope)--;
    
    return valor;
}

void print(STR_COLA* cola) {
    while (!isEmpty(*cola)) {
        int valor = remove(cola);
        printf("valor: %d\t", valor);
    }
}

