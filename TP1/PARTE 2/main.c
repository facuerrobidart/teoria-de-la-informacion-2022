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


typedef struct nodoDiccionario {
    char pal[8];
    char cod[20];
    int ocurr;
} nodoDiccionario;

#define ALT_MAXIMA 50

struct NodoHuff {
    char pal[8];
    unsigned ocur;
    struct NodoHuff *izq, *der;
};

struct TSubArbol {
    unsigned tamano;
    unsigned capacidad;
    struct NodoHuff **array;
};

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
    cincoSimbolos();
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
        //printf("Kraft: %0.4f \n", kraft);
    }

    printf("Cantidad de palabras: %d \n", tamLista);

    if (kraft <= 1){
        printf("El codigo de tamano %d cumple la inecuacion de Kraft/MacMillan (es compacto)\n", tamPalabra);
    }
    else {
        printf("El codigo de tamano %d NO cumple la inecuacion de Kraft/MacMillan (es compacto)\n", tamPalabra);
    }
}

// Escribe un array
void mostrarArray(int arr[], int n) {
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);

    printf("\n");
}

void insertarEnDiccionario(nodoDiccionario *diccionario, int arr[], int n) {
    int i;

    for (i = 0; i < n; ++i) {
        diccionario->cod[i] = '0' + arr[i];
    }
    diccionario->cod[n] = '\0';
}


// Crea un nodo del arbol de Huffman
struct NodoHuff *creaNodo(char pal[], unsigned ocur) {
    struct NodoHuff *temp = (struct NodoHuff *)malloc(sizeof(struct NodoHuff));

    temp->izq = temp->der = NULL;
    strcpy(temp->pal, pal);
    temp->ocur = ocur;

    return temp;
}

// Crea la lista de sub Arboles
struct TSubArbol *crearSubArboles(unsigned capacidad) {
    struct TSubArbol *subArboles = (struct TSubArbol *)malloc(sizeof(struct TSubArbol));

    subArboles->tamano = 0;

    subArboles->capacidad = capacidad;

    subArboles->array = (struct NodoHuff **)malloc(subArboles->capacidad * sizeof(struct NodoHuff *));
    return subArboles;
}

// Intercambia los nodos
void cambiaNodoMinimo(struct NodoHuff **a, struct NodoHuff **b) {
    struct NodoHuff *t = *a;
    *a = *b;
    *b = t;
}

// Busca el nodo con menor frecuencia
void minSubArbol(struct TSubArbol *subArboles, int idx) {
    int min = idx;
    int izq = 2 * idx + 1;
    int der = 2 * idx + 2;

    if (izq < subArboles->tamano && subArboles->array[izq]->ocur < subArboles->array[min]->ocur)
        min = izq;

    if (der < subArboles->tamano && subArboles->array[der]->ocur < subArboles->array[min]->ocur)
        min = der;

    if (min != idx) {
        cambiaNodoMinimo(&subArboles->array[min], &subArboles->array[idx]);
        minSubArbol(subArboles, min);
    }
}

// Revisa si hay un solo subArbol
int hayUno(struct TSubArbol *minHeap) {
    return (minHeap->tamano == 1);
}

// Obtiene el minimo nodo
struct NodoHuff *obtenerMinimo(struct TSubArbol *minHeap) {
    struct NodoHuff *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->tamano - 1];

    --minHeap->tamano;
    minSubArbol(minHeap, 0);

    return temp;
}

// Insertion function
void insertMinHeap(struct TSubArbol *subArboles, struct NodoHuff *minHeapNode) {
    ++subArboles->tamano;
    int i = subArboles->tamano - 1;

    while (i && minHeapNode->ocur < subArboles->array[(i - 1) / 2]->ocur) {
        subArboles->array[i] = subArboles->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    subArboles->array[i] = minHeapNode;
}

void construyeSubArboles(struct TSubArbol *subArboles) {
    int n = subArboles->tamano - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minSubArbol(subArboles, i);
}

int esHoja(struct NodoHuff *raiz) {
    return !(raiz->izq) && !(raiz->der);
}

struct TSubArbol *creaSubArboles(nodoProb lista[], int tamLista) {
    struct TSubArbol *subArboles = crearSubArboles(tamLista);

    for (int i = 0; i < tamLista; ++i)
        subArboles->array[i] = creaNodo(lista[i].pal, lista[i].ocurrencia);

    subArboles->tamano = tamLista;
    construyeSubArboles(subArboles);

    return subArboles;
}

struct NodoHuff *construirArbolDeHuffman(nodoProb item[], int size) {
    struct NodoHuff *left, *right, *tope;
    struct TSubArbol *subArboles = creaSubArboles(item, size);

    while (!hayUno(subArboles)) {
        left = obtenerMinimo(subArboles);
        right = obtenerMinimo(subArboles);

        tope = creaNodo("", left->ocur + right->ocur);

        tope->izq = left;
        tope->der = right;

        insertMinHeap(subArboles, tope);
    }
    return obtenerMinimo(subArboles);
}


//Recorre el Arbol de Huffman y arma los codigos en arr
void escribirCodigosHuffman(struct NodoHuff *raiz, int arr[], int tope, nodoDiccionario diccionario[], int *contador) {
    if (raiz->izq) {
        arr[tope] = 0;
        escribirCodigosHuffman(raiz->izq, arr, tope + 1, diccionario, contador);
    }
    if (esHoja(raiz)) {
        printf("  %s   | ", raiz->pal);
        strcpy(diccionario[*contador].pal, raiz->pal);
        insertarEnDiccionario(&diccionario[*contador], arr, tope);
        diccionario[*contador].ocurr = raiz->ocur;
        (*contador)++;
        mostrarArray(arr, tope);
    }
    if (raiz->der) {
        arr[tope] = 1;
        escribirCodigosHuffman(raiz->der, arr, tope + 1, diccionario, contador);
    }
}

void ordenaDiccionario(nodoDiccionario diccionario[], int tamDiccionario) {
    nodoDiccionario aux;

    printf("-------------------------------------------------\n");
    printf("Ordenando Diccionario......\n");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < tamDiccionario; i++) {
        for (int j = i + 1; j < tamDiccionario; j++) {
            if (diccionario[i].ocurr < diccionario[j].ocurr) {
                aux = diccionario[i];
                diccionario[i] = diccionario[j];
                diccionario[j] = aux;
            }
        }
    }

}

void mostrarDiccionario(nodoDiccionario diccionario[], int tamDiccionario) {

    printf("-------------------------------------------------\n");
    printf("Diccionario:\n");
    for(int i = 0; i < tamDiccionario; i++) {
        printf("PALABRA: %8s CODIGO: %15s OCURRENCIA: %6d\n", diccionario[i].pal, diccionario[i].cod, diccionario[i].ocurr);
    }
}

void ejecutaHuffman(nodoProb lista[], int tamLista) {
    struct NodoHuff *root = construirArbolDeHuffman(lista, tamLista);

    int arr[ALT_MAXIMA], top = 0;
    nodoDiccionario diccionario[tamLista];
    int contador = 0;

    escribirCodigosHuffman(root, arr, top, diccionario, &contador);
    ordenaDiccionario(diccionario, tamLista);
    mostrarDiccionario(diccionario, tamLista);
}
