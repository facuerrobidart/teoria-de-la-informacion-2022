#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct nodoProb
{
    char pal[8];
    int ocurrencia;
    double prob;
} nodoProb;

typedef struct nodoHuffman {
    char pal[8];
    int ocur;
    int sumado;
    struct nodoHuffman *izq;
    struct nodoHuffman *der;
} nodoHuffman;

typedef struct nodoDiccionario {
    char pal[8];
    char cod[8];
} nodoDiccionario;

void tresSimbolos();
void cincoSimbolos();
void sieteSimbolos();
void leerArchivo();
void buscaYCuenta(nodoProb lista[], char *pal, int *tamLista);
void mostrarResultados(nodoProb lista[], int tamPalabra, int tamLista, int totalPalabras,float* entropia);
void iniciaLista(nodoProb lista[]);
void calculaKraft(int tamLista, int tamPalabra);
void ejecutaHuffman(nodoProb lista[], int tamLista);

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

float rendimiento(float entropia, float longitudMedia) {
    return entropia / longitudMedia;
}
float reduncancia(float entropia, float longitudMedia) {
    return 1 - rendimiento(entropia, longitudMedia);
}


void leerArchivo(int tamPalabra) {
    FILE * arch = fopen("text.txt", "r");
    char pal[8];
    nodoProb lista[3500];
    int tamLista = 0;
    int contPalabras = 0;
    iniciaLista(lista);
    double entropia=0,redundancia_=0,rendimiento_=0;
    while (!feof(arch)) {
        fgets(pal, (tamPalabra + 1) * sizeof(char), arch);
        if (strlen(pal) == tamPalabra) {
            buscaYCuenta(lista, pal, &tamLista);
            contPalabras++;
        }
    }

    mostrarResultados(lista, tamPalabra, tamLista, contPalabras,&entropia);
    redundancia_=reduncancia(entropia,tamPalabra);
    rendimiento_=rendimiento(entropia,tamPalabra);
    printf("redundancia: %f , rendimiento:  %f",redundancia_,rendimiento_);
    fclose(arch);
}

void iniciaLista(nodoProb lista[]) {
    for (int i = 0; i < 3500; i++) {
        lista[i].ocurrencia = 0;
        lista[i].prob = 0;
        strcpy(lista[i].pal, "");
    }
}

void buscaYCuenta(nodoProb lista[], char *pal, int *tamLista){
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

void mostrarResultados(nodoProb lista[], int tamPalabra, int tamLista, int cantPalabras,float * entropia) {
    double cantidadDeInformacion = 0;

    for (int i = 0; i < tamLista - 1; i++) {
            lista[i].prob = (double) lista[i].ocurrencia / cantPalabras;
            cantidadDeInformacion += log2(1/lista[i].prob);
            (*entropia)+= lista[i].prob* log2(1/lista[i].prob);
            //printf("Palabra: %s Ocurrencias: %4d Probalilidad: %0.4f \n", lista[i].pal, lista[i].ocurrencia, lista[i].prob);
    }

    printf("cantidad de info:  %f \n",cantidadDeInformacion);
    calculaKraft(tamLista, tamPalabra);
    ejecutaHuffman(lista, tamLista);
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

void recorrerInOrden(nodoHuffman *raiz) {
    if (raiz != NULL) {
        recorrerInOrden(raiz->izq);
        if (raiz->izq == NULL && raiz->der == NULL) {
            printf("PALABRA: %s OCURRENCIAS: %d\n", raiz->pal, raiz->ocur);
        }
        recorrerInOrden(raiz->der);
    }
}

//tamano tiene que ser invocado inicializado en cero
void armarDiccionario(nodoHuffman *raiz, nodoDiccionario diccionario[], char codigo[], int *tamanoDiccionario, char caracterAgregado) {
    if (caracterAgregado != '\0') { //excluye la invocacion inicial
        codigo[strlen(codigo)] = caracterAgregado; //agrega el carcater al codigo
    }

    if (raiz->der == NULL && raiz->izq == NULL) { //si es hoja, hay que agregarlo al diccionario
        strcpy(diccionario[*tamanoDiccionario].pal, raiz->pal);
        strcpy(diccionario[*tamanoDiccionario].cod, codigo);
        (*tamanoDiccionario)++;
    }

    if (raiz->izq != NULL) {
        armarDiccionario(raiz->izq, diccionario, codigo, tamanoDiccionario, '0');
    }
    if (raiz->der != NULL) {
        armarDiccionario(raiz->der, diccionario, codigo, tamanoDiccionario, '1');
    }

    codigo[strlen(codigo)] = '\0'; //elimina el caracter que recien agregue
}

int encontrarMinimo(nodoHuffman **bosque, int tamLista, int excluir) {
    int smaller;
    int i = 0;

    while (bosque[i]->sumado == 1)
        i++;

    smaller=i;

    if (i == excluir){
        i++;
        while (bosque[i]->sumado==1)
            i++;
        smaller=i;
    }

    for (i=0;i<tamLista;i++){
        if (bosque[i]->sumado == 1)
            continue;
        if (i==excluir)
            continue;
        if (bosque[i]->ocur<bosque[smaller]->ocur)
            smaller = i;
    }

    return smaller;
}

// La idea básica es armar un array de punteros a subarboles y agrupar los que tengan menor
// probabilidad de ocurrencia hasta que quede uno solo, que será el arbol de Huffman
void armarArbol(nodoProb lista[], int tamLista, nodoHuffman **arbol) {
    nodoHuffman *aux;
    int i = 0, cantSubArboles = tamLista;
    int menor1, menor2;

    while (i < tamLista) {
        aux = (nodoHuffman *) malloc(sizeof(nodoHuffman));
        strcpy(aux->pal, lista[i].pal);
        aux->ocur = lista[i].ocurrencia;
        aux->izq = NULL;
        aux->der = NULL;
        aux->sumado = 0;
        arbol[i] = aux;
        i++;
    }

    while (cantSubArboles > 1) {
        menor1 = encontrarMinimo(arbol, tamLista, -1);
        menor2 = encontrarMinimo(arbol, tamLista, menor1);
        aux = arbol[menor1];
        arbol[menor1] = malloc(sizeof(nodoHuffman));

        strcpy(arbol[menor1]->pal, "");
        arbol[menor1]->ocur = aux->ocur + arbol[menor2]->ocur;
        arbol[menor1]->izq = arbol[menor2];
        arbol[menor1]->der = aux;
        arbol[menor1]->sumado = 0;
        arbol[menor2]->sumado = 1;

        cantSubArboles--;
    }

    *arbol = arbol[menor1];
}

void ejecutaHuffman(nodoProb lista[], int tamLista) {
    nodoHuffman *raiz = NULL;
    armarArbol(lista, tamLista, &raiz);
    recorrerInOrden(raiz);
}
