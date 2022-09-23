#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nodo
{
    char pal[8];
    int ocurrencia;
} nodo;

void tresSimbolos();
void cincoSimbolos();
void sieteSimbolos();
void leerArchivo();
void buscaYCuenta(nodo lista[], char *pal, int *tamLista);
void mostrarResultados(nodo lista[], int strLen);
void iniciaLista(nodo lista[]);

int main(){
    sieteSimbolos();
    return 0;
}

void tresSimbolos() {
    leerArchivo(3);
}

void cincoSimbolos() {
    leerArchivo(5);
}

void sieteSimbolos() {
    leerArchivo(7);
}

void leerArchivo(int tamPalabra) {
    FILE * arch = fopen("text.txt", "r");
    char pal[8];
    nodo lista[3500];
    int *tamLista = 0;
    iniciaLista(lista);

    while (!feof(arch)) {
        fgets(pal, (tamPalabra + 1) * sizeof(char), arch);
        buscaYCuenta(lista, pal, &tamLista);
    }

    mostrarResultados(lista, tamPalabra);

    fclose(arch);
}

void iniciaLista(nodo lista[]) {
    for (int i = 0; i < 3500; i++) {
        lista[i].ocurrencia = 0;
        strcpy(lista[i].pal, "");
    }
}

void buscaYCuenta(nodo lista[], char *pal, int *tamLista){
    int encontrado = 0, i = 0;

    while (!encontrado && (i < *tamLista)){
        if (strcmp(lista[i].pal, pal) == 0){
            encontrado = 1;
        }
        i++;
    }

    if (!encontrado){
        strcpy(lista[i].pal, pal);
        lista[i].ocurrencia = 1;
        *tamLista = i + 1;
    }
    else {
        lista[i].ocurrencia++;
    }
}

void mostrarResultados(nodo lista[], int strLen) {
    for (int i = 0; i < 3500; i++) {
        if (lista[i].ocurrencia != 0 && strlen(lista[i].pal) == strLen) {
            printf("Palabra: %s Ocurrencias: %d \n", lista[i].pal, lista[i].ocurrencia);
        }
    }
}