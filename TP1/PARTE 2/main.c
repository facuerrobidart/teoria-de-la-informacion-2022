#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct nodo
{
    char pal[8];
    int ocurrencia;
    double prob;
} nodo;

void tresSimbolos();
void cincoSimbolos();
void sieteSimbolos();
void leerArchivo();
void buscaYCuenta(nodo lista[], char *pal, int *tamLista);
void mostrarResultados(nodo lista[], int tamPalabra, int tamLista, int totalPalabras);
void iniciaLista(nodo lista[]);
void calculaKraft(int tamLista, int tamPalabra);

int main(){
    tresSimbolos();
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
    int tamLista = 0;
    int contPalabras = 0;
    iniciaLista(lista);

    while (!feof(arch)) {
        fgets(pal, (tamPalabra + 1) * sizeof(char), arch);
        if (strlen(pal) == tamPalabra) {
            buscaYCuenta(lista, pal, &tamLista);
            contPalabras++;
        }
    }

    mostrarResultados(lista, tamPalabra, tamLista, contPalabras);

    fclose(arch);
}

void iniciaLista(nodo lista[]) {
    for (int i = 0; i < 3500; i++) {
        lista[i].ocurrencia = 0;
        lista[i].prob = 0;
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
        (*tamLista)++;
    }
    else {
        lista[i].ocurrencia++;
    }
}

void mostrarResultados(nodo lista[], int tamPalabra, int tamLista, int cantPalabras) {
    double cantidadDeInformacion = 0;
    for (int i = 0; i < tamLista - 1; i++) {
            lista[i].prob = (double) lista[i].ocurrencia / cantPalabras;
            cantidadDeInformacion += log2(1/lista[i].prob);
            //printf("Palabra: %s Ocurrencias: %4d Probalilidad: %0.4f \n", lista[i].pal, lista[i].ocurrencia, lista[i].prob);
    }

    calculaKraft(tamLista, tamPalabra);
}

void calculaKraft(int tamLista, int tamPalabra) {
    double kraft = 0;
    for (int k = 0; k< tamLista; k++){
        kraft += pow(3, (-1) * tamPalabra);
        printf("Kraft: %0.4f \n", kraft);
    }

    printf("Cantidad de palabras: %d \n", tamLista);

    if (kraft <= 1){
        printf("El codigo de tamano %d cumple la inecuacion de Kraft/MacMillan (es compacto)\n", tamPalabra);
    }
    else {
        printf("El codigo de tamano %d NO cumple la inecuacion de Kraft/MacMillan (es compacto)\n", tamPalabra);
    }
}

float rendimiento(float entropia, float longitudMedia) {
    return entropia / longitudMedia;
}
float reduncancia(float entropia, float longitudMedia) {
    return 1 - rendimiento(entropia, longitudMedia);
}