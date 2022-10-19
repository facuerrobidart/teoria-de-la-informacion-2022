#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct nodoProb
{
    char pal[8];
    int ocurrencia;
    float prob;
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

typedef struct formatoDiccionario {
    char pal[8];
    unsigned short cod;
} formatoDiccionario;

void tresSimbolos();
void cincoSimbolos();
void sieteSimbolos();
void leerArchivo();
void buscaYCuenta(nodoProb lista[], char *pal, int *tamLista);
void mostrarResultados(nodoProb lista[], int tamPalabra, int tamLista, int totalPalabras,float* entropia);
void iniciaLista(nodoProb lista[]);
void calculaKraft(int tamLista, float longitudMedia);
void ejecutaHuffman(nodoProb lista[], int tamLista, int tamPalabra);

int main(){
    int opcion = 0;
    while(opcion <= 0 || opcion >= 4){
        printf("Seleccione una opcion: \n\n");
        printf("1 - Procesar archivo con palabras de 3 caracteres\n");
        printf("2 - Procesar archivo con palabras de 5 caracteres\n");
        printf("3 - Procesar archivo con palabras de 7 caracteres\n");
        scanf("%d", &opcion);
    }

    switch (opcion) {
        case (1):
            tresSimbolos();
            break;
        case (2):
            cincoSimbolos();
            break;
        case (3):
            sieteSimbolos();
            break;
        default:
            break;
    }
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

float calculaLongitudMedia(nodoProb lista[], int tamLista){
  float longitudMedia=0;
  for(int i = 0; i < tamLista ; i++){
   longitudMedia += lista[i].prob * strlen(lista[i].pal);
  }
  return longitudMedia;
}

float rendimiento(float entropia, float longitudMedia) {
    return entropia/longitudMedia;
}

float reduncancia(float entropia, float longitudMedia) {
    return 1 - rendimiento(entropia, longitudMedia);
}


void leerArchivo(int tamPalabra) {
    printf("-------------------------------------------------\n");
    printf("Leyendo archivo con palabras de %d caracteres\n", tamPalabra);

    FILE * arch = fopen("text.txt", "r");
    char pal[8];
    nodoProb lista[3500];
    int tamLista = 0;
    int contPalabras = 0;
    iniciaLista(lista);
    float longitudMedia=0;
    float entropia=0,redundancia_=0,rendimiento_=0;
    while (!feof(arch)) {
        fgets(pal, (tamPalabra + 1) * sizeof(char), arch);
        if (strlen(pal) == tamPalabra) {
            buscaYCuenta(lista, pal, &tamLista);
            contPalabras++;
        }
    }
    printf("-------------------------------------------------\n");


    mostrarResultados(lista, tamPalabra, tamLista, contPalabras,&entropia);
   //printf("LA ENTROPIA ES: %f \n",entropia);
    longitudMedia= (float) tamPalabra; //calculaLongitudMedia(lista,tamLista);
    redundancia_=reduncancia(entropia,longitudMedia);
    rendimiento_=rendimiento(entropia,longitudMedia);
    printf("Redundancia: %0.2f \nRendimiento:  %0.2f \n", redundancia_, rendimiento_);
    printf("-------------------------------------------------\n");
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

//como todas las palabras son de igual longitud, basta con analizar la compactitud de una sola palabra
void compactitud(int tamPalabra) {
    float probabilidadPalabra = 1.0 / pow(3, tamPalabra);
    float resultado = log2(1/probabilidadPalabra)/log2(3); //esto es un logaritmo base 3, usamos cambio de base

    if ((int) resultado == tamPalabra) {
        printf("El codigo es compacto\n");
    }
    else {
        printf("El codigo no es compacto\n");
    }
}

void mostrarResultados(nodoProb lista[], int tamPalabra, int tamLista, int cantPalabras,float * entropia) {
    float cantidadDeInformacion = 0,longitudMedia;

    for (int i = 0; i < tamLista ; i++) {
            lista[i].prob = (float) lista[i].ocurrencia / cantPalabras;
            cantidadDeInformacion += log2(1/lista[i].prob);
            (*entropia)= (*entropia) + lista[i].prob* log2(1/lista[i].prob)/log2(3);
            //tuvimos que aplicarle cambio de base al log, ya que C no tiene definido log3
    }
    longitudMedia = tamPalabra; //si bien hay un calculo de longitud media, todas las palabras son de la misma longitud por definicion
    printf("Cantidad de informacion:  %0.2f \n", cantidadDeInformacion);
    printf("Longitud media: %0.2f \n",longitudMedia);
    printf("Entropia: %0.2f \n", (*entropia));
    calculaKraft(tamLista, longitudMedia);
    compactitud(tamPalabra);
    ejecutaHuffman(lista, tamLista, tamPalabra);
}

void calculaKraft(int tamLista, float longitudMedia) {
    float kraft = 0;
    for (int k = 0; k< tamLista; k++){
        kraft += pow(3, (-1) * longitudMedia);
    }

    printf("Cantidad de palabras: %d \n", tamLista);

    printf("Resultado Kraft/McMillan: %0.2f \n", kraft);

    if (kraft <= 1){
        printf("El codigo de longitud media %f cumple la inecuacion de Kraft/MacMillan\n", longitudMedia);
    }
    else {
        printf("El codigo de longitud media %f NO cumple la inecuacion de Kraft/MacMillan\n", longitudMedia);
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
int hayUno(struct TSubArbol *subarboles) {
    return (subarboles->tamano == 1);
}

// Obtiene el minimo nodo
struct NodoHuff *obtenerMinimo(struct TSubArbol *subArboles) {
    struct NodoHuff *temp = subArboles->array[0];
    subArboles->array[0] = subArboles->array[subArboles->tamano - 1];

    --subArboles->tamano;
    minSubArbol(subArboles, 0);

    return temp;
}

void insertarNuevoNodo(struct TSubArbol *subArboles, struct NodoHuff *minimoNodo) {
    ++subArboles->tamano;
    int i = subArboles->tamano - 1;

    while (i && minimoNodo->ocur < subArboles->array[(i - 1) / 2]->ocur) {
        subArboles->array[i] = subArboles->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    subArboles->array[i] = minimoNodo;
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
    struct NodoHuff *izq, *der, *tope;
    struct TSubArbol *subArboles = creaSubArboles(item, size);

    while (!hayUno(subArboles)) {
        izq = obtenerMinimo(subArboles);
        der = obtenerMinimo(subArboles);

        tope = creaNodo("", izq->ocur + der->ocur);

        tope->izq = izq;
        tope->der = der;

        insertarNuevoNodo(subArboles, tope);
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
        strcpy(diccionario[*contador].pal, raiz->pal);
        insertarEnDiccionario(&diccionario[*contador], arr, tope);
        diccionario[*contador].ocurr = raiz->ocur;
        (*contador)++;
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

char *buscaPalabra(nodoDiccionario diccionario[], int tamDiccionario, char palabra[]) {
    for (int i = 0; i < tamDiccionario; i++) {
        if ( strcmp(palabra, diccionario[i].pal) == 0) {
            return diccionario[i].cod;
        }
    }
    return "";
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void generaArchCodificado(nodoDiccionario diccionario[], int tamDiccionario, int tamPalabra) {
    FILE *archCodificado;
    FILE *archOriginal;
    archOriginal = fopen("text.txt", "r+");
    char sufijo[10] = "";
    itoa(tamPalabra, sufijo, 10);
    char filename[50] = "archCodificado";
    strcat(filename, sufijo);
    strcat(filename, ".bin");
    archCodificado = fopen( filename, "wb");
    char pal[8];

    printf("-------------------------------------------------\n");
    printf("Generando archivo codificado......\n");

    formatoDiccionario aux;


    fwrite(&tamDiccionario, sizeof(int), 1, archCodificado); //Los primeros 4 bytes son el tamaño del diccionario

    for (int i = 0; i < tamDiccionario; i++) { //Escribo el diccionario, en total son 8 bytes por palabra y 2 bytes por codigo, por lo que el tamaño del diccionario es 10 bytes por palabra
        strcpy(aux.pal, diccionario[i].pal);
        aux.cod = strtol(diccionario[i].cod, NULL, 2);
        fwrite(&aux, sizeof(formatoDiccionario), 1, archCodificado);
    }

    while (!feof(archOriginal)) { //Escribo el texto codificado
        fgets(pal, (tamPalabra + 1) * sizeof(char), archOriginal);
        pal[tamPalabra] = '\0';
        if (strlen(pal) == tamPalabra) { //valida no leer la ultima palabra
            unsigned short codificado = (unsigned  short) strtol(buscaPalabra(diccionario, tamDiccionario, pal), NULL, 2);
            fwrite(&codificado, 2, 1, archCodificado);
        }
    }

    printf("-------------------------------------------------\n");
    printf("Archivo codificado generado con exito!\n");
    printf("-------------------------------------------------\n");
    fclose(archOriginal);
    fclose(archCodificado);
}

void mostrarDiccionario(nodoDiccionario diccionario[], int tamDiccionario) {

    printf("-------------------------------------------------\n");
    printf("Diccionario:\n");
    for(int i = 0; i < tamDiccionario; i++) {
        printf("PALABRA: %8s CODIGO: %15s OCURRENCIA: %6d\n", diccionario[i].pal, diccionario[i].cod, diccionario[i].ocurr);
    }
}

char *buscarTraduccion(formatoDiccionario diccionario[], int tamDiccionario, unsigned short codigo) {
    for (int i = 0; i < tamDiccionario; i++) {
        if (codigo == diccionario[i].cod) {
            return diccionario[i].pal;
        }
    }
    return "";
}

void reconstruirTexto(int tamPalabra) {
    char filename[50] = "archCodificado";
    char sufijo[10] = "";
    itoa(tamPalabra, sufijo, 10);
    strcat(filename, sufijo);
    strcat(filename, ".bin");

    printf("-------------------------------------------------\n");
    printf("Reconstruyendo texto......\n");
    FILE *archCodificado = fopen(filename, "rb");

    int tamDiccionario;
    fread(&tamDiccionario, sizeof(int), 1, archCodificado);

    formatoDiccionario diccionario[tamDiccionario];


    for (int i = 0; i < tamDiccionario; i++) {
        fread(&diccionario[i], sizeof(formatoDiccionario), 1, archCodificado);
    }

    int cont = 0;
    while (!feof(archCodificado)) {
        cont++;
        unsigned short codigo;
        fread(&codigo, sizeof(unsigned short), 1, archCodificado);
        printf("%s", buscarTraduccion(diccionario, tamDiccionario, codigo));
        if (cont == 10) {
            printf("\n");
            cont = 0;
        }
    }

    printf("\n-------------------------------------------------\n");
}

void ejecutaHuffman(nodoProb lista[], int tamLista, int tamPalabra) {
    struct NodoHuff *root = construirArbolDeHuffman(lista, tamLista);

    int arr[ALT_MAXIMA], top = 0;
    nodoDiccionario diccionario[tamLista];
    int contador = 0;
    escribirCodigosHuffman(root, arr, top, diccionario, &contador);
    ordenaDiccionario(diccionario, tamLista);
    mostrarDiccionario(diccionario, tamLista);
    generaArchCodificado(diccionario, tamLista, tamPalabra);

    printf("Presione 1 si desea reconstruir el texto, 0 para salir: ");
    int opcion = 5;
    while (opcion != 0 && opcion != 1) {
        scanf("%d", &opcion);
    }
    if (opcion == 1) {
        reconstruirTexto(tamPalabra);
    }
}
