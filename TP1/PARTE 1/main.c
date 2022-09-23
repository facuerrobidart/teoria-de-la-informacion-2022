#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void inicializar(int N, float probabilidades[N][N],float VecEst[N]);
void calcularProbabilidades(int N, float probabilidades[N][N]);
void mostrarMatriz(int N, float probabilidades[N][N]);
int esErgodico(int N, float probabilidades[N][N]);
void creaMGauss(int N, float MGauss[N+1][N+1], float probabilidades[N][N]);
void Gauss(int N, float MGauss[N+1][N+1],float VEst[N]);
float Entropia(int N, float probabilidades[N][N], float VecEst[N]);

int main()
{
    int N = 3;
    float probabilidades[N][N];
    float VecEst[N],MGauss[N+1][N+1];

    inicializar(N,probabilidades,VecEst);
    calcularProbabilidades(N,probabilidades);
    mostrarMatriz(N,probabilidades);

    printf("\n");
    if(esErgodico(N,probabilidades))
    {
        printf("El grafo es ergodico.\n\n");
        float MGauss[N+1][N+1];
        creaMGauss(N,MGauss,probabilidades);
        Gauss(N,MGauss,VecEst);
        printf("Vector Estacionario:\n");
        for(int i = 0; i < N; i++)
            printf("\t%f\n",VecEst[i]);
        printf("\nLa entropia es %f bits/simbolo\n",Entropia(N,probabilidades,VecEst));
    }
    else
        printf("El grafo no es ergodico.");

    return 0;
}

void inicializar(int N, float probabilidades[][N], float VecEst[])
{
    for (int i = 0; i < N; i++)
    {
        VecEst[i] = 0;
        for (int j = 0; j < N; j++)
            probabilidades[i][j] = 0;
    }
}

void calcularProbabilidades(int N, float probabilidades[][N])
{
    FILE * arch = fopen("text.txt", "r");
    float ocurrencias[N][N];
    int cont[N];

    for (int i = 0; i < N; i++)
    {
        cont[i] = 0;
        for (int k = 0; i < N; i++)
        {
            ocurrencias[i][k] = 0;
        }
    }

    char prev, act;
    fscanf(arch, "%c", &prev);
    fscanf(arch, "%c", &act);
    cont[act-'A'] = 1;
    while (!feof(arch))
    {
        ocurrencias[act-'A'][prev-'A']++;
        prev = act;
        fscanf(arch, "%c", &act);
        cont[act-'A']++;
    }

    printf("Cantidad de veces que aparece cada caracter:\n");
    for(int i = 0; i < N; i++)
        printf("%c: %d\n",i+'A',cont[i]);
    printf("Ocurrencias:\n");
    printf("\tA\tB\tC\n");

    for (int i = 0; i < N; i++)
    {
        printf("\t");
        for (int j = 0; j < N; j++)
        {
            printf("%0.0f\t",ocurrencias[i][j]);
        }
        printf("\n");
    }
    printf("--------------:\n");


    for (int i = 0; i < N; i++)
    {
        for (int k = 0; k < N; k++)
        {
            probabilidades[i][k] = ocurrencias[i][k] / (float) cont[k];
        }
    }

    fclose(arch);
}

void mostrarMatriz(int N, float probabilidades[][N])
{
    printf("Matriz de pasaje:\n");
    printf("\tA\tB\tC\n");

    for (int i = 0; i < N; i++)
    {
        printf("\t");
        for (int j = 0; j < N; j++)
            printf("%0.2f\t",probabilidades[i][j]);
        printf("\n");
    }
}

int esErgodico(int N, float probabilidades[][N])
{
    int M[N][N];
    int fila=0, columna=0;
    for(int i = 0; i < N; i++)
        for(int j= 0; j< N; j++)
            M[i][j] = 0;
    for(int k = 0; k < N; k++)
        for(int i = 0; i < N; i++)
            for(int j = 0; j < N; j++)
                if(probabilidades[i][j] > 0 || (probabilidades[i][k] + probabilidades[k][j] > 0))
                    M[i][j] = 1;
    while(fila < N)
    {
        while(columna < N)
        {
            if(M[fila][columna] != 1)
                return 0;
            columna++;
        }
        fila++;
        columna = 0;
    }
    return 1;
}

void creaMGauss(int N, float MGauss[][N+1], float probabilidades[][N])
{
    for(int i = 0; i < N+1; i++)
        for(int j = 0; j < N+1; j++)
            if(i == 0)
                MGauss[i][j] = 1;
            else if (j == N)
                MGauss[i][j] = 0;
            else
            {
                MGauss[i][j] = probabilidades[i-1][j];
                if(i == j+1)
                    MGauss[i][j]--;
            }

}

void Gauss(int N, float M[][N+1],float V[])
{
    float aux1, aux2;
    for(int i = 0; i < N+1; i++)
        for(int j = i+1; j < N+1; j++)
        {
            aux1=M[j][i]/M[i][i];
            for(int k = i; k < N+1; k++)
            {
                aux2=M[i][k]*aux1;
                M[j][k]-=aux2;
            }
        }

    for(int i = N-1; i>=0; i--)
    {
        for(int j = N-1; j>i; j--)
            V[i] -= M[i][j]*V[j];

        V[i] = (V[i]+M[i][N])/M[i][i];
    }
}

float Entropia(int N, float probabilidades[][N], float VecEst[])
{
    float H = 0, aux;

    for(int i = 0; i < N; i++)
    {
        aux = 0;
        for(int j = 0; j < N; j++)
        {
            if(probabilidades[j][i]!=0)
                aux-=probabilidades[j][i]*(log(probabilidades[j][i])/log(2));
        }
        H+=VecEst[i]*aux;
    }
    return H;
}


