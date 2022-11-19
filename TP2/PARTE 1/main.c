#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>

#define ALT_MAXIMA 200

struct NodoHuff {
    char pal[25];
    unsigned ocur;
    struct NodoHuff *izq, *der;
};

struct TSubArbol {
    unsigned tamano;
    unsigned capacidad;
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

typedef struct nodoShannon {
    char pal[25];
    float pro;
    int arr[20];
    int top;
    int ocurrencia;
} nodoShannon;

void huffman();
void shannonFano();
void buscaYCuenta(nodoProb lista[], char *pal, int *tamLista);
void leerArchivo(nodoProb lista[], int *tamLista, int *contPalabras);

int main() {
    int opcion = 0;

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
}



void shannon(int l, int h, nodoShannon s[]) {
    float pack1=0,pack2=0,diff1=0,diff2=0;
    int i, k, j;
    if((l+1)==h || l==h || l>h) {
        if(l==h || l>h)
            return;
        s[h].arr[++(s[h].top)]=0;
        s[l].arr[++(s[l].top)]=1;
        return;
    }
    else {
        for(i=l;i <= h-1;i++)
            pack1=pack1+s[i].pro;
        pack2 = pack2 + s[h].pro;
        diff1 = pack1 - pack2;

        if(diff1< 0)
            diff1=diff1*-1;
        j=2;

        while(j != h-l+1) {
            k = h - j;
            pack1 = pack2 = 0;

            for(i=l;i<=k;i++)
                pack1 = pack1 + s[i].pro;

            for(i=h;i>k;i--)
                pack2 = pack2 + s[i].pro;

            diff2 = pack1 - pack2;

            if(diff2 < 0)
                diff2 = diff2*-1;

            if(diff2>=diff1)
                break;

            diff1 = diff2;
            j++;
        }
        k++;

        for(i=l;i <= k;i++)
            s[i].arr[++(s[i].top)]=1;

        for(i=k+1;i <= h;i++)
            s[i].arr[++(s[i].top)]=0;

        shannon(l,k,s);
        shannon(k+1,h,s);
    }
}

void transformarANodoShannon(nodoProb lista[], nodoShannon s[], int tamLista) {
    for (int i = 0; i < tamLista; i++) {
        strcpy(s[i].pal, lista[i].pal);
        s[i].pro = lista[i].prob;
        s[i].top = -1;
        s[i].ocurrencia = lista[i].ocurrencia;
    }
}

void ordenarShannon(nodoShannon s[], int tamLista) {
    for (int i = 0; i < tamLista; i++) {
        for (int j = 0; j < tamLista - 1; j++) {
            if (s[j].pro < s[j + 1].pro) {
                nodoShannon aux = s[j];
                s[j] = s[j + 1];
                s[j + 1] = aux;
            }
        }
    }
}


void nodoShannonADiccionario(nodoShannon s[], nodoDiccionario diccionario[], int tamLista) {
    for (int i = 0; i < tamLista; i++) {
        strcpy(diccionario[i].pal, s[i].pal);
        diccionario[i].ocurr = s[i].ocurrencia;
        for (int j = 0; j <= s[i].top; j++) {
            diccionario[i].cod[j] = s[i].arr[j] + '0';
        }
        diccionario[i].cod[s[i].top] = '\0';
    }
}

void shannonFano() {
    nodoProb *lista = (nodoProb *) malloc(sizeof(nodoProb) * 10000);
    nodoShannon *s = (nodoShannon *) malloc(sizeof(nodoShannon) * 10000);
    int tamLista = 0;
    int contPalabras = 0;
    leerArchivo(lista, &tamLista, &contPalabras);
    transformarANodoShannon(lista, s, tamLista);

    free(lista);

    ordenarShannon(s, tamLista);
    shannon(0, tamLista - 1, s);

    nodoDiccionario *diccionario = (nodoDiccionario *) malloc(sizeof(nodoDiccionario) * tamLista);
    nodoShannonADiccionario(s, diccionario, tamLista);
    free(s);

    ordenaDiccionario(diccionario, tamLista);

    int opcion = 5;
    while (opcion != 0 && opcion != 1) {
        printf("Presione 1 si desea mostrar el diccionario, 0 para seguir: ");
        scanf("%d", &opcion);
    }
    if (opcion == 1) {
        mostrarDiccionario(diccionario, tamLista);
    }

    generaArchCodificado(diccionario, tamLista, 0);

    opcion = 5;
    printf("Presione 1 si desea reconstruir el texto, 0 para salir: ");
    while (opcion != 0 && opcion != 1) {
        scanf("%d", &opcion);
    }
    if (opcion == 1) {
        reconstruirTexto(0);
    }
}