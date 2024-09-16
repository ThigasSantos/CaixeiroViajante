#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <chrono>


using namespace std;
using namespace std::chrono;

#ifdef _OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#endif

#define NUM_THREADS 15

// Função para calcular a distância euclidiana entre dois pontos
double calcularDistancia(int ponto1[2], int ponto2[2])
{
    int dx = ponto1[0] - ponto2[0];
    int dy = ponto1[1] - ponto2[1];
    return sqrt(dx * dx + dy * dy);
}

// Função para calcular a menor distância usando uma variação do algoritmo de TSP
double tsp(double **distancias, int *visitado, int atual, int count, double custoAtual, double *menorCusto, int *caminhoAtual, int *melhorCaminho, int nos, int pontoFinal)
{
    if (count == nos - 1)
    {
        custoAtual += distancias[atual][pontoFinal];
        caminhoAtual[count] = pontoFinal;
        if (custoAtual < *menorCusto)
        {
            *menorCusto = custoAtual;
            for (int i = 0; i <= count; ++i)
                melhorCaminho[i] = caminhoAtual[i];
        }
        return *menorCusto;
    }

    for (int i = 0; i < nos; ++i)
    {
        if (!visitado[i] && i != pontoFinal)
        {
            visitado[i] = 1;
            caminhoAtual[count] = i;
            tsp(distancias, visitado, i, count + 1, custoAtual + distancias[atual][i], menorCusto, caminhoAtual, melhorCaminho, nos, pontoFinal);
            visitado[i] = 0;
        }
    }

    return *menorCusto;
}

int main()
{
    auto start_completo = high_resolution_clock::now();
    char nome[256];
    FILE *arqEntrada;
    FILE *arqSaida;

    printf("Informar nome do arquivo de entrada: ");
    scanf("%s", nome);
    arqEntrada = fopen(nome, "r");

    if (arqEntrada == NULL)
    {
        printf("Erro ao abrir arquivo de entrada.\n");
        return 1;
    }

    arqSaida = fopen("caminhoParalelo.txt", "w");
    if (arqSaida == NULL)
    {
        printf("Erro ao criar arquivo de saída.\n");
        return 1;
    }

    int quantidadeNos, quantidadeIniciais;
    fscanf(arqEntrada, "%d\n", &quantidadeNos);
    fscanf(arqEntrada, "%d\n", &quantidadeIniciais);

    int **coordenadas = (int **)malloc(quantidadeNos * sizeof(int *));
    for (int i = 0; i < quantidadeNos; ++i)
    {
        coordenadas[i] = (int *)malloc(2 * sizeof(int));
        fscanf(arqEntrada, "%d %d\n", &coordenadas[i][0], &coordenadas[i][1]);
    }
    fclose(arqEntrada);

    int pontoFinal;
    printf("Informar o número do ponto que deve ser o último a ser visitado: ");
    scanf("%d", &pontoFinal);
    pontoFinal--; // Ajuste para índice 0

    double **distancias = (double **)malloc(quantidadeNos * sizeof(double *));
    for (int i = 0; i < quantidadeNos; ++i)
        distancias[i] = (double *)malloc(quantidadeNos * sizeof(double));

    // Calcula as distâncias entre os pontos
    int tid;
#pragma omp parallel for schedule(dynamic) num_threads(NUM_THREADS) shared(distancias, coordenadas, quantidadeNos) private(tid) default(none)
    for (int i = 0; i < quantidadeNos; ++i)
    {
        for (int j = 0; j < quantidadeNos; ++j)
        {
            if (i != j)
            {
                distancias[i][j] = calcularDistancia(coordenadas[i], coordenadas[j]);
#pragma omp critical
                {
                    tid = omp_get_thread_num();
                    printf("Thread: %d calculou a distância entre os pontos %d e %d\n", tid, i + 1, j + 1);
                }
            }
            else
            {
                distancias[i][j] = INFINITY;
            }
        }
    }

    // Paralelização das execuções da função tsp
#pragma omp parallel for schedule(dynamic) num_threads(NUM_THREADS) shared(distancias, quantidadeNos, pontoFinal, quantidadeIniciais, arqSaida) private(tid) default(none)
    for (int inicio = 0; inicio < quantidadeIniciais; ++inicio)
    {
        tid = omp_get_thread_num();
        printf("Thread: %d iniciando TSP para ponto inicial %d\n", tid, inicio + 1);

        int *visitado = (int *)malloc(quantidadeNos * sizeof(int));
        int *caminhoAtual = (int *)malloc(quantidadeNos * sizeof(int));
        int *melhorCaminho = (int *)malloc(quantidadeNos * sizeof(int));
        double menorCusto = INFINITY;

        auto start = high_resolution_clock::now();

        for (int i = 0; i < quantidadeNos; i++)
            visitado[i] = 0;

        visitado[inicio] = 1;
        caminhoAtual[0] = inicio;
        tsp(distancias, visitado, inicio, 1, 0, &menorCusto, caminhoAtual, melhorCaminho, quantidadeNos, pontoFinal);

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

#pragma omp critical
        {
            printf("Thread %d finalizou TSP para ponto inicial %d com menor custo %.2f\n", tid, inicio + 1, menorCusto);
            fprintf(arqSaida, "Menor distância começando do ponto %d: %.2f\n", inicio + 1, menorCusto);
            fprintf(arqSaida, "Caminho percorrido: ");
            for (int i = 0; i < quantidadeNos; ++i)
                fprintf(arqSaida, "%d ", melhorCaminho[i] + 1);
            fprintf(arqSaida, "\nTempo de execução: %lld microssegundos\n\n", duration.count());
        }

        // Libera a memória alocada para cada thread
        free(visitado);
        free(caminhoAtual);
        free(melhorCaminho);
    }

    // Libera a memória alocada para as distâncias e coordenadas
    for (int i = 0; i < quantidadeNos; ++i)
    {
        free(coordenadas[i]);
        free(distancias[i]);
    }
    free(coordenadas);
    free(distancias);

    auto end_completo = high_resolution_clock::now();
    auto duration_completo = duration_cast<microseconds>(end_completo - start_completo);
    fprintf(arqSaida, "Tempo total de execução: %lld microssegundos\n", duration_completo.count());

    fclose(arqSaida);
    return 0;
}
