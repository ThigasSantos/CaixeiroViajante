#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Função para calcular a distância euclidiana entre dois pontos
double calcularDistancia(pair<int, int> ponto1, pair<int, int> ponto2)
{
    int dx = ponto1.first - ponto2.first;
    int dy = ponto1.second - ponto2.second;
    return sqrt(dx * dx + dy * dy);
}

// Função para calcular a menor distância usando uma variação do algoritmo de TSP
double tsp(const vector<vector<double>> &distancias, vector<bool> &visitado, int atual, int count, double custoAtual, double &menorCusto, vector<int> &caminhoAtual, vector<int> &melhorCaminho, int nos, int pontoFinal)
{
    if (count == nos - 1)
    {
        custoAtual += distancias[atual][pontoFinal];
        caminhoAtual.push_back(pontoFinal);
        if (custoAtual < menorCusto)
        {
            menorCusto = custoAtual;
            melhorCaminho = caminhoAtual;
        }
        caminhoAtual.pop_back();
        return menorCusto;
    }

    for (int i = 0; i < nos; ++i)
    {
        if (!visitado[i] && i != pontoFinal)
        {
            visitado[i] = true;
            caminhoAtual.push_back(i);
            tsp(distancias, visitado, i, count + 1, custoAtual + distancias[atual][i], menorCusto, caminhoAtual, melhorCaminho, nos, pontoFinal);
            caminhoAtual.pop_back();
            visitado[i] = false;
        }
    }

    return menorCusto;
}

int main()
{
    FILE *arqEntrada;
    FILE *arqSaida;
    char nome[256];

    cout << "Informar nome do arquivo de entrada: ";
    cin >> nome;
    arqEntrada = fopen(nome, "r");

    if (arqEntrada == NULL)
    {
        cout << "Erro ao abrir arquivo de entrada." << endl;
        return 1;
    }

    arqSaida = fopen("caminho.txt", "w");
    if (arqSaida == NULL)
    {
        cout << "Erro ao criar arquivo de saída." << endl;
        return 1;
    }

    int quantidadeNos, quantidadeIniciais;
    fscanf(arqEntrada, "%d\n", &quantidadeNos);
    fscanf(arqEntrada, "%d\n", &quantidadeIniciais);

    vector<pair<int, int>> coordenadas(quantidadeNos);
    for (int i = 0; i < quantidadeNos; ++i)
    {
        fscanf(arqEntrada, "%d %d\n", &coordenadas[i].first, &coordenadas[i].second);
    }
    fclose(arqEntrada);

    int pontoFinal;
    cout << "Informar o número do ponto que deve ser o último a ser visitado: ";
    cin >> pontoFinal;
    pontoFinal--; // Ajuste para índice 0

    vector<vector<double>> distancias(quantidadeNos, vector<double>(quantidadeNos));
    for (int i = 0; i < quantidadeNos; ++i)
    {
        for (int j = 0; j < quantidadeNos; ++j)
        {
            if (i != j)
            {
                distancias[i][j] = calcularDistancia(coordenadas[i], coordenadas[j]);
            }
            else
            {
                distancias[i][j] = numeric_limits<double>::infinity();
            }
        }
    }

    for (int inicio = 0; inicio < quantidadeIniciais; ++inicio)
    {
        auto start = high_resolution_clock::now();
        vector<bool> visitado(quantidadeNos, false);
        vector<int> caminhoAtual;
        vector<int> melhorCaminho;
        visitado[inicio] = true;
        caminhoAtual.push_back(inicio);
        double menorCusto = numeric_limits<double>::infinity();
        tsp(distancias, visitado, inicio, 1, 0, menorCusto, caminhoAtual, melhorCaminho, quantidadeNos, pontoFinal);

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

        // Exibindo e salvando os resultados
        cout << "Menor distância começando do ponto " << inicio + 1 << ": " << menorCusto << endl;
        cout << "Caminho percorrido: ";
        fprintf(arqSaida, "Menor distância começando do ponto %d: %.2f\n", inicio + 1, menorCusto);
        fprintf(arqSaida, "Caminho percorrido: ");
        for (int i : melhorCaminho)
        {
            cout << i + 1 << " ";
            fprintf(arqSaida, "%d ", i + 1);
        }
        cout << endl;
        fprintf(arqSaida, "\nTempo de execução: %lld microssegundos\n", duration.count());
        cout << "Tempo de execução: " << duration.count() << " microssegundos" << endl;
    }

    fclose(arqSaida);
    return 0;
}
