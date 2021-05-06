// ImplementacionPilaVector.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int CAPACIDAD_PILA = 10;

typedef struct Pila {
    int tope;//cantidad de elementos pila
    int elementos[CAPACIDAD_PILA];//vector donde se almacenan los valores
}STR_PILA;

void push(int dato, STR_PILA* pila);
int pop(STR_PILA* pila);
void create(STR_PILA* pila);
int peek(STR_PILA pila);
bool isEmpty(STR_PILA pila);
void print(STR_PILA* pila);

int main()
{
    srand(time(NULL));
    STR_PILA pila;
    create(&pila);
    int i = 0;
    char confirmar = 'n';
    
    printf("Desea agregar un valor aleatorio a la pila? [S/N]: [ ]\b\b");
    scanf(" %c", &confirmar);
    
    while ((confirmar == 'S' || confirmar == 's') && i<CAPACIDAD_PILA) {
              
        push(rand() % 1000/*i*/, &pila);
        if (pila.tope<CAPACIDAD_PILA) {
            printf("Desea agregar un valor aleatorio a la pila? [S/N]: [ ]\b\b");
            scanf(" %c", &confirmar);
        }
        else {
            printf("No puede agregar mas elementos a la pila\n");
            printf("Tope=capacidad de la pila= %d\n", pila.tope);
        }
        i++;
    }
    
    print(&pila);
    printf("\nPila vacia\n");
    return 0;
}

void create(STR_PILA* pila)
{
    pila->tope = 0;
    return;
}

void push(int dato, STR_PILA* pila)
{
    if (pila->tope < CAPACIDAD_PILA) {
        pila->elementos[pila->tope] = dato;
        (pila->tope)++;
        
    }
}

bool isEmpty(STR_PILA pila)
{
    return pila.tope == 0;
}

int peek(STR_PILA pila)
{
    return pila.elementos[pila.tope-1];
}

int pop(STR_PILA* pila)
{
    int valor = pila->elementos[pila->tope-1];
    (pila->tope)--;
    return valor;
}

void print(STR_PILA* pila) {
    while (!isEmpty(*pila)) {
        int valor = pop(pila);
        printf("valor: %d\n", valor);
    }
}
