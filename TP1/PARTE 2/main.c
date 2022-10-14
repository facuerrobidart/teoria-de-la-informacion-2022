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
    longitudMedia=calculaLongitudMedia(lista,tamLista);
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



void mostrarResultados(nodoProb lista[], int tamPalabra, int tamLista, int cantPalabras,float * entropia) {
    float cantidadDeInformacion = 0,longitudMedia;

    for (int i = 0; i < tamLista ; i++) {
            lista[i].prob = (float) lista[i].ocurrencia / cantPalabras;
            cantidadDeInformacion += log2(1/lista[i].prob);
            (*entropia)= (*entropia) + lista[i].prob* log2(1/lista[i].prob)/log2(3);
            //tuvimos que aplicarle cambio de base al log, ya que C no tiene definido log3
    }
    longitudMedia=calculaLongitudMedia(lista,tamLista);
    printf("Cantidad de informacion:  %0.2f \n", cantidadDeInformacion);
    printf("Longitud media: %0.2f \n",longitudMedia);
    printf("Entropia: %0.2f \n", (*entropia));
    calculaKraft(tamLista, longitudMedia);
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
        printf("El codigo de longitud media %f cumple la inecuacion de Kraft/MacMillan  (es compacto)\n", longitudMedia);
    }
    else {
        printf("El codigo de longitud media %f NO cumple la inecuacion de Kraft/MacMillan (es compacto)\n", longitudMedia);
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

void insertarMinSub(struct TSubArbol *subArboles, struct NodoHuff *minimoNodo) {
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

        insertarMinSub(subArboles, tope);
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

void generaArchCodificado(nodoDiccionario diccionario[], int tamDiccionario, int tamPalabra) {
    FILE *archCodificado;
    FILE *archOriginal;
    archOriginal = fopen("text.txt", "r+");
    char filename[50] = "archCodificado.bin";
    archCodificado = fopen( filename, "wb");
    char pal[8];

    printf("-------------------------------------------------\n");
    printf("Generando archivo codificado......\n");

    while (!feof(archOriginal)) {
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

void ejecutaHuffman(nodoProb lista[], int tamLista, int tamPalabra) {
    struct NodoHuff *root = construirArbolDeHuffman(lista, tamLista);

    int arr[ALT_MAXIMA], top = 0;
    nodoDiccionario diccionario[tamLista];
    int contador = 0;

    escribirCodigosHuffman(root, arr, top, diccionario, &contador);
    ordenaDiccionario(diccionario, tamLista);
    mostrarDiccionario(diccionario, tamLista);
    generaArchCodificado(diccionario, tamLista, tamPalabra);
}
