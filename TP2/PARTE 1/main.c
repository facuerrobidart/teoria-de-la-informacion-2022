#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define ALT_MAXIMA 200

struct NodoHuff {
    char pal[25];
    int ocur;
    struct NodoHuff *izq, *der;
};

struct TSubArbol {
    int tamano;
    int capacidad;
    struct NodoHuff **array;
};

typedef struct nodoDiccionario {
    char pal[25];
    char cod[20];
    int ocurr;
} nodoDiccionario;

typedef struct formatoDiccionario {
    char pal[25];
    unsigned short cod;
} formatoDiccionario;

typedef struct nodoProb
{
    char pal[25];
    int ocurrencia;
    float prob;
} nodoProb;


typedef struct {
    uint8_t* PunteroByte;
    uint32_t PosicionBit;
} BitStream;

typedef struct {
    uint32_t Palabra;
    uint32_t Cont;
    uint32_t Codigo;
    uint32_t Bits;
} Simbolo;

typedef struct treeNode {
    struct treeNode* Izq;
    struct treeNode* Der;
    int Palabra;
} NodoArbolShannon;

#define CANT_MAXIMA_NODOS 511

void huffman();
void shannonFano();
void buscaYCuenta(nodoProb lista[], char *pal, int *tamLista);
void leerArchivo(nodoProb lista[], int *tamLista, int *contPalabras);
void stats();

int main() {
    int opcion = 0;

    stats();
    printf("-------------------------------------------------\n");
    while (opcion <= 0 || opcion >= 3) {
        printf("Ingrese una opcion: \n");
        printf("1 - Codificacion de Huffman\n");
        printf("2 - Codificacion de Shennon-Fano\n");
        scanf("%d", &opcion);
    }

    if (opcion == 1) {
        huffman();
    } else {
        shannonFano();
    }
    return 0;
}

void iniciaLista(nodoProb lista[]) {
    for (int i = 0; i < 5000; i++) {
        lista[i].ocurrencia = 0;
        lista[i].prob = 0;
        strcpy(lista[i].pal, "");
    }
}

/*void buscaYCuenta(nodoProb lista[], char *pal, int *tamLista){
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
}*/

void buscaYCuenta(nodoProb *lista, char *pal, int *tamLista) {
    int i = 0;

    while (strcmp(lista[i].pal, pal) != 0 && i < (*tamLista)) {
        i++;
    }

    if (strcmp(lista[i].pal, pal) == 0) {
        lista[i].ocurrencia++;
    } else {
        strcpy(lista[i].pal, pal);
        lista[i].ocurrencia = 1;
        (*tamLista)++;
    }
}

void leerArchivo(nodoProb lista[], int *tamLista, int *contPalabras) {
    printf("-------------------------------------------------\n");
    printf("Leyendo archivo de entrada\n");

    FILE * arch = fopen("text.txt", "r");
    char pal[25];
    iniciaLista(lista);
    while (!feof(arch)) {
        fscanf(arch, "%s", pal);
        buscaYCuenta(lista, pal, tamLista);
        (*contPalabras)++;
    }

    for (int i = 0; i < *tamLista; i++) {
        lista[i].prob = (float) lista[i].ocurrencia / ((float) (*contPalabras));
    }

    printf("-------------------------------------------------\n");
    fclose(arch);
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
        diccionario->cod[i] =  '0' + arr[i];
    }
    diccionario->cod[n] = '\0';
}


// Crea un nodo del arbol de Huffman
struct NodoHuff *creaNodo(char pal[], int ocur) {
    struct NodoHuff *temp = (struct NodoHuff *)malloc(sizeof(struct NodoHuff));

    temp->izq = NULL;
    temp->der = NULL;
    strcpy(temp->pal, pal);
    temp->ocur = ocur;

    return temp;
}

// Crea la lista de sub Arboles
struct TSubArbol *crearSubArboles(int capacidad) {
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

void mostrarDiccionario(nodoDiccionario diccionario[], int tamDiccionario) {
    printf("-------------------------------------------------\n");
    printf("Diccionario:\n");
    for(int i = 0; i < tamDiccionario; i++) {
        printf("PALABRA: %8s CODIGO: %15s OCURRENCIA: %6d\n", diccionario[i].pal, diccionario[i].cod, diccionario[i].ocurr);
    }
}

void generaArchCodificado(nodoDiccionario diccionario[], int tamDiccionario, int esHuffman) {
    FILE *archCodificado;
    FILE  *archOriginal = fopen("text.txt", "r+");
    char filename[50] = "archCodificado";

    if (esHuffman)
        strcat(filename, ".huf");
    else
        strcat(filename, ".sha");

    archCodificado = fopen( filename, "wb");
    char pal[25];

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
        fscanf(archOriginal, "%s", pal);
        unsigned short codificado = (unsigned  short) strtol(buscaPalabra(diccionario, tamDiccionario, pal), NULL, 2);
        fwrite(&codificado, 2, 1, archCodificado);
    }

    printf("-------------------------------------------------\n");
    printf("Archivo codificado generado con exito!\n");
    printf("-------------------------------------------------\n");
    fclose(archOriginal);
    fclose(archCodificado);
}

char *buscarTraduccion(formatoDiccionario diccionario[], int tamDiccionario, unsigned short codigo) {
    for (int i = 0; i < tamDiccionario; i++) {
        if (codigo == diccionario[i].cod) {
            return diccionario[i].pal;
        }
    }
    return "";
}


void reconstruirTexto(int esHuffman) {
    char filename[50] = "archCodificado";
    if (esHuffman)
        strcat(filename, ".huf");
    else
        strcat(filename, ".sha");

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
        printf("%s ", buscarTraduccion(diccionario, tamDiccionario, codigo));
        if (cont == 10) {
            printf("\n");
            cont = 0;
        }
    }

    printf("\n-------------------------------------------------\n");
}


void huffman() {
    nodoProb *lista = (nodoProb *) malloc(sizeof(nodoProb) * 10000);
    int tamLista = 0;
    int contPalabras = 0;
    leerArchivo(lista, &tamLista, &contPalabras);

    struct NodoHuff *root = construirArbolDeHuffman(lista, tamLista);

    int arr[ALT_MAXIMA], top = 0;
    free(lista);
    nodoDiccionario diccionario[tamLista];
    int contador = 0;
    escribirCodigosHuffman(root, arr, top, diccionario, &contador);
    ordenaDiccionario(diccionario, tamLista);

    int opcion = 5;
    printf("-------------------------------------------------\n");
    printf("Presione 1 para mostrar el diccionario, 0 para seguir: ");
    while (opcion != 0 && opcion != 1) {
        scanf("%d", &opcion);
    }

    if (opcion == 1) {
        mostrarDiccionario(diccionario, tamLista);
    }

    generaArchCodificado(diccionario, tamLista, 1);

    opcion = 5;
    printf("Presione 1 si desea reconstruir el texto, 0 para salir: ");
    while (opcion != 0 && opcion != 1) {
        scanf("%d", &opcion);
    }
    if (opcion == 1) {
        reconstruirTexto(1);
    }

    FILE * archOriginal = fopen("text.txt", "r");
    fseek(archOriginal, 0L, SEEK_END);
    int tamOriginal = ftell(archOriginal);
    fclose(archOriginal);

    FILE * archCodificado = fopen("archCodificado.huf", "rb");
    fseek(archCodificado, 0L, SEEK_END);
    int tamCodificado = ftell(archCodificado);
    fclose(archCodificado);

    printf("-------------------------------------------------\n");
    printf("Tamano original: %d bytes\n", tamOriginal);
    printf("Tamano codificado: %d bytes\n", tamCodificado);
    printf("Tasa de compresion: %f\n", (float) tamOriginal / (float) tamCodificado);
    printf("-------------------------------------------------\n");

}

// Los proximos metodos son para la codificacion de Shannon fano
void initBitStream(BitStream* stream, uint8_t* buffer) {
    stream->PunteroByte = buffer;
    stream->PosicionBit = 0;
}


uint32_t leerBit(BitStream* stream) {
    uint8_t* buffer = stream->PunteroByte;
    uint32_t bit = stream->PosicionBit;
    uint32_t x = (*buffer & (1 << (7 - bit))) ? 1 : 0;
    bit = (bit + 1) & 7;

    if (!bit) {
        ++buffer;
    }

    stream->PosicionBit = bit;
    stream->PunteroByte = buffer;

    return x;
}

uint32_t leerByte(BitStream* stream)
{
    uint8_t* buffer = stream->PunteroByte;
    uint32_t bit = stream->PosicionBit;
    uint32_t x = (*buffer << bit) | (buffer[1] >> (8 - bit));
    ++buffer;
    stream->PunteroByte = buffer;

    return x;
}

NodoArbolShannon *armarArbol(NodoArbolShannon* nodos, BitStream* stream, uint32_t* nro) {
    NodoArbolShannon* esteNodo;

    esteNodo = &nodos[*nro];
    *nro = *nro + 1;

    esteNodo->Palabra = -1;
    esteNodo->Izq = (NodoArbolShannon*)0;
    esteNodo->Der = (NodoArbolShannon*)0;

    if (leerBit(stream)){
        esteNodo->Palabra = leerByte(stream);
        return esteNodo;
    }

    if (leerBit(stream)){
        esteNodo->Izq = armarArbol(nodos, stream, nro);
    }

    if (leerBit(stream))
    {
        esteNodo->Der = armarArbol(nodos, stream, nro);
    }

    return esteNodo;
}

void DescomprimirShannon(uint8_t* entrada, uint8_t* salida, uint32_t tamanoEntrada, uint32_t tamanoSalida) {
    NodoArbolShannon nodos[CANT_MAXIMA_NODOS], *raiz, *nodo;
    BitStream stream;
    uint32_t i, conteo;
    uint8_t* buffer;

    if (tamanoEntrada < 1) return;

    initBitStream(&stream, entrada);

    conteo = 0;
    raiz = armarArbol(nodos, &stream, &conteo);
    buffer = salida;

    for (i = 0; i < tamanoSalida; ++i)
    {
        nodo = raiz;

        while (nodo->Palabra < 0)
        {
            if (leerBit(&stream))
                nodo = nodo->Der;
            else
                nodo = nodo->Izq;
        }

        *buffer++ = (uint8_t)nodo->Palabra;
    }
    *buffer++ = (uint8_t) "\0"; //agrego el caracter de fin de string
}


void escribirBits(BitStream* stream, uint32_t x, uint32_t bits)
{
    uint8_t* buffer = stream->PunteroByte;
    uint32_t bit = stream->PosicionBit;
    uint32_t mask = 1 << (bits - 1);

    for (uint32_t count = 0; count < bits; ++count)
    {
        *buffer = (*buffer & (0xff ^ (1 << (7 - bit)))) + ((x & mask ? 1 : 0) << (7 - bit));
        x <<= 1;
        bit = (bit + 1) & 7;

        if (!bit) {
            ++buffer;
        }
    }

    stream->PunteroByte = buffer;
    stream->PosicionBit = bit;
}

void histograma(uint8_t* entrada, Simbolo* simbolo, uint32_t tamano) {
    Simbolo temp;
    int i, cambio;

    for (i = 0; i < 256; ++i) {
        simbolo[i].Palabra = i;
        simbolo[i].Cont = 0;
        simbolo[i].Codigo = 0;
        simbolo[i].Bits = 0;
    }

    for (i = tamano; i; --i) {
        simbolo[*entrada++].Cont++;
    }

    do {
        cambio = 0;
        for (i = 0; i < 255; ++i) {
            if (simbolo[i].Cont < simbolo[i + 1].Cont) {
                temp = simbolo[i];
                simbolo[i] = simbolo[i + 1];
                simbolo[i + 1] = temp;
                cambio = 1;
            }
        }
    } while (cambio);
}

void crearArbolShannon(Simbolo* simbolo, BitStream* stream, uint32_t codigo, uint32_t bits, uint32_t primero, uint32_t ultimo) {
    uint32_t i, size, sizeA, sizeB, lastA, firstB;

    if (primero == ultimo) {
        escribirBits(stream, 1, 1);
        escribirBits(stream, simbolo[primero].Palabra, 8);
        simbolo[primero].Codigo = codigo;
        simbolo[primero].Bits = bits;
        return;
    }
    else {
        escribirBits(stream, 0, 1);
    }

    size = 0;

    for (i = primero; i <= ultimo; ++i) {
        size += simbolo[i].Cont;
    }

    sizeA = 0;

    for (i = primero; sizeA < ((size + 1) >> 1) && i < ultimo; ++i) {
        sizeA += simbolo[i].Cont;
    }

    if (sizeA > 0) {
        escribirBits(stream, 1, 1);

        lastA = i - 1;

        crearArbolShannon(simbolo, stream, (codigo << 1) + 0, bits + 1, primero, lastA);
    }
    else {
        escribirBits(stream, 0, 1);
    }

    sizeB = size - sizeA;

    if (sizeB > 0) {
        escribirBits(stream, 1, 1);

        firstB = i;

        crearArbolShannon(simbolo, stream, (codigo << 1) + 1, bits + 1, firstB, ultimo);
    }
    else {
        escribirBits(stream, 0, 1);
    }
}

int ComprimirShannon(uint8_t* entrada, uint8_t* salida, uint32_t tamanoEntrada) {
    Simbolo sym[256], temp;
    BitStream stream;
    uint32_t i, bytesTotales, cambio, simbolo, ultimoSimbolo;

    if (tamanoEntrada < 1)
        return 0;

    initBitStream(&stream, salida);
    histograma(entrada, sym, tamanoEntrada);

    for (ultimoSimbolo = 255; sym[ultimoSimbolo].Cont == 0; --ultimoSimbolo);

    if (ultimoSimbolo == 0)
        ++ultimoSimbolo;

    crearArbolShannon(sym, &stream, 0, 0, 0, ultimoSimbolo);

    do {
        cambio = 0;

        for (i = 0; i < 255; ++i)
        {
            if (sym[i].Palabra > sym[i + 1].Palabra)
            {
                temp = sym[i];
                sym[i] = sym[i + 1];
                sym[i + 1] = temp;
                cambio = 1;
            }
        }
    } while (cambio);

    for (i = 0; i < tamanoEntrada; ++i) {
        simbolo = entrada[i];
        escribirBits(&stream, sym[simbolo].Codigo, sym[simbolo].Bits);
    }

    bytesTotales = (int)(stream.PunteroByte - salida);

    if (stream.PosicionBit > 0) {
        ++bytesTotales;
    }

    return bytesTotales;
}

void generarCodificadoShannonFano(uint8_t* codificado) {
    FILE * archCodificado = fopen("archCodificado.sha", "wb+");
    fprintf(archCodificado, "%s", codificado);
    fclose(archCodificado);
}

void shannonFano() {
    FILE * arch = fopen("text.txt", "r");
    char str[100000];
    char buffer[20];

    printf("Leyendo archivo...\n");
    while (!feof(arch)) {
        fgets(buffer, sizeof(buffer), arch);
        strcat(str, buffer);
    }
    printf("Archivo leido, comprimiendo...\n");
    uint8_t* original = (uint8_t*)str;
    int tamOriginal = strlen(str);
    uint8_t* comprimido = (uint8_t*)malloc(tamOriginal * (101 / 100) + 384);

    int tamComprimido = ComprimirShannon(original, comprimido, tamOriginal);
    printf("Tamano comprimido: %d bytes\n", tamComprimido);
    printf("Tamano sin comprimir: %d bytes\n", tamOriginal);
    printf("Tasa de compresion: %f\n", (float)tamOriginal / (float) tamComprimido);
    printf("Exportando archivo...\n");
    generarCodificadoShannonFano(comprimido);

    int opcion = 3;
    printf("Ingrese 1 para reconstruir el archivo, 0 para salir\n");

    while (opcion != 1 && opcion != 0) {
        scanf("%d", &opcion);
    }

    if (opcion == 1) {
        uint8_t *reconstruccion = (uint8_t *) malloc(tamOriginal);
        DescomprimirShannon(comprimido, reconstruccion, tamComprimido, tamOriginal);

        printf("Reconstruccion: \n");
        printf("%s", reconstruccion);
        free(reconstruccion);
    }

    free(comprimido);
}

int calculaLongPalabra(char * palabra){
    int i =0, longitud=0;
    while(palabra[i] != '\0'){
        longitud++;
        i++;
    }
    return longitud;
}


float calculaLongMedia(nodoProb *lista, int tamLista){
    float longMedia = 0;
    for(int i=0;i<tamLista;i++){
        longMedia+= lista[i].prob * ((float) (calculaLongPalabra(lista[i].pal)));
    }
    return longMedia;

}

float calculaEntropia(nodoProb *lista,int tamLista){
    float entropia = 0;
    for(int i=0;i<tamLista;i++){
        entropia = entropia + lista[i].prob  * ( log(1/lista[i].prob)/ log(68) )  ;
    }
    return entropia;

}

float calculaRendimiento(float entropia,float longitudMedia){
    return entropia/longitudMedia;
}


float calculaRedundancia(float rendimiento){
    return 1-rendimiento;
}

void stats() {
    nodoProb *lista = (nodoProb *) malloc(10000 * sizeof(nodoProb));
    int tamLista = 0;
    int contPalabras = 0;

    leerArchivo(lista, &tamLista, &contPalabras);

    float longMedia = calculaLongMedia(lista, tamLista);
    float entropia = calculaEntropia(lista, tamLista);
    float rendimiento = calculaRendimiento(entropia, longMedia);
    float redundancia = calculaRedundancia(rendimiento);

    printf("Longitud media: %f\n", longMedia);
    printf("Entropia: %f\n", entropia);
    printf("Rendimiento: %f\n", rendimiento);
    printf("Redundancia: %f\n", redundancia);

    free(lista);
}