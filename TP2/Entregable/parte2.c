#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void leerArch(char [],int *, int *, float [], float [][10]);
void desarrollo(int, int, float [], float [][10]);
void mostrarMat(char, char, int, int M, float [][10]);
void mostrarVec(char, char,int, float []);
void calculaProbDeSalida(int, int M, float [], float [], float [][10]);
void calcularProbSucSim(int, int M, float [][10], float [], float [][10]);
void calcularProbAPosteriori(int, int M, float [][10], float [][10], float []);
float calcularEntropiaAPriori(int, float []);
void calcularEntropiaAPosteriori(int, int M, float [], float [][10]);
float calcularEquivocacion(int, int M, float [][10], float [][10]);

int main()
{
    int N,M;
    float probEntrada[10];
    float matDelCanal[10][10];
    char tipoArch[5]=".txt";

    for(int i = 0; i < 3; i++)
    {
        printf("//////////////////////// CANAL %d ////////////////////////\n\n\n",i+1);
        char nombreArch[15] = "Canal ", ch = (i+1+'0');
        strncat(nombreArch, &ch, 1);
        strncat(nombreArch, &tipoArch, 4);
        leerArch(nombreArch,&N,&M,probEntrada,matDelCanal);
        desarrollo(N,M,probEntrada,matDelCanal);
        printf("\n\n\n\n\n\n");
    }


}

void leerArch(char nombreArch[15], int *N, int *M, float probEntrada[], float matDelCanal[][10])
{
    FILE * arch;
    arch = fopen(nombreArch,"r");
    fscanf(arch,"%d %d",N,M);
    for(int i = 0; i < *N; i++)
        fscanf(arch,"%f",&probEntrada[i]);
    for(int i = 0; i < *N; i++)
        for(int j = 0; j < *M; j++)
            fscanf(arch,"%f",&matDelCanal[i][j]);

    fclose(arch);
}

void desarrollo(int N, int M, float probEntrada[], float matDelCanal[][10])
{
    float probSalida[10] = {0};
    float probSucSim[10][10];
    float probAPosteriori[10][10];
    float EntropiaAPriori = 0;
    float EntropiaAPosteriori[10] = {0};
    float equivocacion = 0;

    printf("Probabilidad de entrada:\n");
    mostrarVec('P','S',N,probEntrada);
    printf("\nMatriz del canal:\n");
    mostrarMat(' ',' ',N,M,matDelCanal);
    calculaProbDeSalida(N,M,probSalida,probEntrada,matDelCanal);
    printf("\nProbabilidad de salida:\n");
    mostrarVec('P','B',M,probSalida);
    calcularProbSucSim(N,M,probSucSim,probEntrada,matDelCanal);
    printf("\nProbabilidad de sucesos simultaneos:\n");
    mostrarMat('P',',',N,M,probSucSim);
    calcularProbAPosteriori(N,M,probAPosteriori,probSucSim,probSalida);
    printf("\nProbabilidad a posteriori:\n");
    mostrarMat('P','/',N,M,probAPosteriori);
    EntropiaAPriori = calcularEntropiaAPriori(N,probEntrada);
    printf("\nEntropia a priori:\nH(S) = %f\n",EntropiaAPriori);
    calcularEntropiaAPosteriori(N,M,EntropiaAPosteriori,probAPosteriori);
    printf("\nEntropia a posteriori:\n");
    mostrarVec('H','B',M,EntropiaAPosteriori);
    equivocacion = calcularEquivocacion(N,M,probAPosteriori,probSucSim);
    printf("\nEquivocacion:\n H(S/B) = %f\n",equivocacion);
    printf("\nInformacion mutua:\nI(S,B) = %f",EntropiaAPriori-equivocacion);
}

void mostrarMat(char accion,char tipo, int N, int M, float mat[][10])
{
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < M; j++)
            printf("%c(S%d%cB%d) = %f\t",accion,i+1,tipo,j+1,mat[i][j]);
        printf("\n");
    }
}

void mostrarVec(char accion,char tipo, int N, float vec[])
{
    for(int i = 0; i < N; i++)
        printf("%c(%c%d) = %f\n",accion,tipo,i+1,vec[i]);
}

void calculaProbDeSalida(int N, int M, float probSalida[], float probEntrada[], float matDelCanal[][10])
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            probSalida[j]+=probEntrada[i]*matDelCanal[i][j];
}

void calcularProbSucSim(int N, int M, float probSucSim[][10], float probEntrada[], float matDelCanal[][10])
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            probSucSim[i][j]=probEntrada[i]*matDelCanal[i][j];
}

void calcularProbAPosteriori(int N, int M, float probAPosteriori[][10], float probSucSim[][10], float probSalida[])
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            probAPosteriori[i][j]=probSucSim[i][j]/probSalida[j];
}

float calcularEntropiaAPriori(int N, float probEntrada[])
{
    float entropia = 0;
    for(int i = 0; i < N; i++)
    {
        float aux;
        if(probEntrada[i] == 0)
            aux=0;
        else
            aux=log(probEntrada[i]);
        entropia-=probEntrada[i]*aux/log(2);
    }
    return entropia;
}

void calcularEntropiaAPosteriori(int N, int M, float EntropiaAPosteriori[], float probAPosteriori[][10])
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
        {
            float aux;
            if(probAPosteriori[i][j] == 0)
                aux=0;
            else
                aux=log(probAPosteriori[i][j]);
            EntropiaAPosteriori[j]-=probAPosteriori[i][j]*aux/log(2);
        }

}

float calcularEquivocacion(int N, int M, float probAPosteriori[][10], float probSucSim[][10])
{
    float eq=0;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
        {
            float aux;
            if(probAPosteriori[i][j] == 0)
                aux=0;
            else
                aux=log(1/probAPosteriori[i][j]);
            eq+=probSucSim[i][j]*aux/log(2);
        }
    return eq;
}



