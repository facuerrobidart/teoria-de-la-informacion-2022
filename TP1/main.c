#include <stdio.h>
#include <stdlib.h>

void inicializar(float probabilidades[3][3]);
void calcularProbabilidades(float probabilidades[3][3]);
void mostrarMatriz(float probabilidades[3][3]);

int main() {
    float probabilidades[3][3];

    inicializar(probabilidades);
    calcularProbabilidades(probabilidades);
    mostrarMatriz(probabilidades);

    return 0;
}

void inicializar(float probabilidades[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            probabilidades[i][j] = 0;
        }
    }
}

void calcularProbabilidades(float probabilidades[3][3]) {
    FILE * arch = fopen("simbolos.txt", "r+");
    int ocurrencias[3][3];

    for (int i; i < 3; i++) {
        for (int k; i < 3; i++) {
            ocurrencias[i][k] = 0;
        }
    }

    char prev, act;
    int cont = 2;
    fscanf(arch, "%c", &prev);
    fscanf(arch, "%c", &act);
    while (!feof(arch)) {
        ocurrencias[prev-'A'][act-'A']++;
        prev = act;
        fscanf(arch, "%c", &act);
        cont++;
    }

    for (int i; i < 3; i++) {
        for (int k; i < 3; i++) {
            probabilidades[i][k] = ocurrencias[i][k] / cont;
        }
    }

    fclose(arch);
}

void mostrarMatriz(float probablidades[3][3]) {
    printf("Probabilidades:\n");
    printf("\tA\tB\tC");

    printf("A\t%0.2f\t%0.2f\t%0.2f\n", probablidades[0][0], probablidades[0][1], probablidades[0][2]);
    printf("B\t%0.2f\t%0.2f\t%0.2f\n", probablidades[1][0], probablidades[1][1], probablidades[1][2]);
    printf("C\t%0.2f\t%0.2f\t%0.2f\n", probablidades[2][0], probablidades[2][1], probablidades[2][2]);
}
