#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <time.h>	

using namespace std;

#define infinito 0x7FFFFFFF
#define _CRT_SECURE_NO_WARNINGS

struct Cidade {
	int x;
	int y;
};

struct Celula {
	double val;
	vector <int> caminhos;
	Celula* prox;
};

class CidadesVisitas {
private:
	vector <Celula*> ini;
	vector <Celula*> fim;

public:
	CidadesVisitas(int numCidades) {
		ini.resize(numCidades + 1);
		fim.resize(numCidades + 1);
		for (int x = 0; x < numCidades + 1; x++) {
			ini[x] = NULL;
			fim[x] = NULL;
		}
	}

	void inserir(int vertice, double val, vector <int> caminho) {
		Celula* nova = new Celula;
		if (ini[vertice] == NULL) {
			nova->val = val;
			nova->caminhos.resize(caminho.size());
			nova->caminhos = caminho;
			nova->prox = NULL;

			ini[vertice] = nova;
			fim[vertice] = ini[vertice];
		}
		else {
			nova->val = val;
			nova->caminhos.resize(caminho.size());
			nova->caminhos = caminho;
			nova->prox = NULL;

			fim[vertice]->prox = nova;
			fim[vertice] = nova;

			nova = NULL;
		}
	}

	double getVal(int vertice, vector <int> caminho) {
		double resposta = infinito;
		Celula* temp = ini[vertice];
		bool encontrado = true;

		if (caminho.size() == 0) {
			return (ini[vertice]->val);
		}
		else {
			while (temp != NULL) {
				for (int x = 0; x < caminho.size(); x++) {
					if (caminho[x] != temp->caminhos[x]) {
						encontrado = false;
					}
				}

				if (encontrado) {
					resposta = temp->val;
					break;
				}
				encontrado = true;
				temp = temp->prox;
			}
			return (resposta);
		}
	}
};

class TSPD {
private:
	FILE* arqEntrada;
	FILE* arqSaida;
	int n;
	int numPontosIniciais;
	int controle;
	Cidade* cidades;
	vector <int> auxComb;
	vector <int> vetor;
	vector < vector <int> > combinacaoElementos;
	vector < vector <double> > mAdjacentes;
	vector <CidadesVisitas*>  custo;

public:
	TSPD() {
		char nome[256];
		cout << "Informar nome arquivo entrada : ";
		cin >> nome;
		fopen_s(&arqEntrada, nome, "r");
		fopen_s(&arqSaida, "caminho.txt", "w");
		fscanf_s(arqEntrada, "%d\n", &n);
		fscanf_s(arqEntrada, "%d\n", &numPontosIniciais);
		vetor.resize(n - numPontosIniciais);
		cout << "está aqui 1" << endl;
		cidades = new Cidade[n + 1];
		for (int x = 0; x < n; x++) {
			fscanf_s(arqEntrada, "%d %d\n", &cidades[x].x, &cidades[x].y);
		}
		cout << "está aqui 2" << endl;
		fclose(arqEntrada);
		preencherMAdjacentes();
		for (int x = numPontosIniciais; x < n; x++) {
			vetor[x - numPontosIniciais] = x + 1;
			cout << "está aqui 3" << endl;
		}
	}

	void algoritmoTSPD() {
		time_t inicio, fim;
		inicio = clock();
		double custoAux;
		double custoCaminho;
		custo.resize(n - numPontosIniciais);
		vector <int> inicial;

		for (int x = 0; x < numPontosIniciais; x++) {
			inicial.push_back(x + 1);
			cout << "está aqui 4" << endl;
		}

		for (int x = 0; x < n - numPontosIniciais; x++) {
			custo[x] = new CidadesVisitas(n);
			cout << "está aqui 5" << endl;
		}

		for (int x = numPontosIniciais + 1; x <= n; x++) {
			for (int i = 0; i < numPontosIniciais; i++) {
				custo[0]->inserir(x, mAdjacentes[x][inicial[i]], extrair(inicial, inicial[i]));
				cout << "está aqui 6" << endl;
			}
		}

		for (int s = 1; s < (n - numPontosIniciais); s++) {
			for (int x = numPontosIniciais + 1; x < n; x++) {
				cout << "está aqui 7" << endl;
				controle = 0;
				int numComb = calcularNumCombinacoes(extrair(vetor, x).size(), s);
				alocarCombinacoes(numComb, s);
				auxComb.resize(s);
				combinacoes(extrair(vetor, x), s, 0, auxComb);
				for (int j = 0; j < numComb; j++) {
					vector <int> vetAux = removerVetor(s, j);
					custoCaminho = infinito;

					for (int k = 0; k < s; k++) {
						custoAux = mAdjacentes[x][vetAux[k]] + custo[s - 1]->getVal(vetAux[k], extrair(vetAux, vetAux[k]));

						if (custoAux < custoCaminho) {
							custoCaminho = custoAux;
						}
					}
					custo[s]->inserir(x, custoCaminho, vetAux);
				}
			}
		}

		custoCaminho = infinito;
		int cidadeBase = 0;

		for (int x = numPontosIniciais + 1; x <= n; x++) {
			for (int i = 0; i < numPontosIniciais; i++) {
				custoAux = mAdjacentes[inicial[i]][x] + custo[n - numPontosIniciais - 1]->getVal(x, extrair(vetor, x));

				if (custoAux < custoCaminho) {
					custoCaminho = custoAux;
					cidadeBase = x;
				}
			}
		}

		fprintf(arqSaida, "NUMERO CIDADES : %d\nCAMINHO\n%d ", n, cidadeBase);
		recuperarCaminho(cidadeBase, vetor, (n - numPontosIniciais - 1));
		fim = clock();
		fprintf(arqSaida, "\nCUSTO : %0.2f", custoCaminho);
		fprintf(arqSaida, "\nTEMPO : %0.2f", ((double(fim) - double(inicio)) / CLOCKS_PER_SEC));
		fclose(arqSaida);
	}

	~TSPD() {}

private:

	void alocarCombinacoes(int linhas, int colunas) {
		combinacaoElementos.resize(linhas);
		for (int x = 0; x < linhas; x++) {
			combinacaoElementos[x].resize(colunas);
		}
	}

	double calcularDistancia(Cidade primeira, Cidade segunda) {
		double distancia = sqrt(pow((primeira.x - segunda.x), 2) + pow((primeira.y - segunda.y), 2));
		return (distancia);
	}

	void preencherMAdjacentes() {
		mAdjacentes.resize(n);
		for (int x = 0; x < n; x++) {
			mAdjacentes[x].resize(n);
			for (int y = 0; y < n; y++) {
				if (x == y) {
					mAdjacentes[x][y] = 0.0;
				}
				else {
					mAdjacentes[x][y] = calcularDistancia(cidades[x], cidades[y]);
				}
			}
		}
		delete[] cidades;  // Corrigido para liberar memória alocada dinamicamente
	}


	vector <int> extrair(vector <int> vet, int elemento) {
		vector <int> resposta;
		resposta.resize(vet.size() - 1);
		int pos = 0;
		for (int x = 0; x < vet.size(); x++) {
			if (vet[x] != elemento) {
				resposta[pos++] = vet[x];
			}
		}
		return (resposta);
	}

	int calcularNumCombinacoes(int numEle, int tamConj) {
		int combinacaoDividendo = 1;
		int combinacaDivisor = 1;
		int aux = tamConj;
		for (int x = 0; x < tamConj; x++) {
			if (numEle != aux) {
				combinacaoDividendo *= numEle--;
			}
			else {
				combinacaoDividendo *= numEle;
				numEle--;
			}
			combinacaDivisor *= aux--;
		}
		return (combinacaoDividendo / combinacaDivisor);
	}

	vector <int> removerVetor(int tam, int linha) {
		vector <int> vetAux;
		vetAux.resize(tam);
		for (int x = 0; x < tam; x++) {
			vetAux[x] = combinacaoElementos[linha][x];
		}
		return (vetAux);
	}

	void combinacoes(vector <int> vet, int tamConj, int indice, vector <int> auxComb) {
		if (tamConj == 0) {
			for (int x = 0; x < auxComb.size(); x++) {
				combinacaoElementos[controle][x] = auxComb[x];
			}
			controle++;
		}
		else {
			for (int x = indice; x <= vet.size() - tamConj; x++) {
				auxComb[auxComb.size() - tamConj] = vet[x];
				combinacoes(vet, tamConj - 1, x + 1, auxComb);
			}
		}
	}

	void recuperarCaminho(int cidadeBase, vector <int> vet, int tam) {
		vector <int> vetAux;

		if (tam >= 0) {
			int numComb = calcularNumCombinacoes(vet.size(), tam);
			for (int y = 0; y < numComb; y++) {
				for (int x = 0; x < tam; x++) {
					if (cidadeBase == combinacaoElementos[y][x]) {
						for (int i = 0; i < tam; i++) {
							vetAux.push_back(combinacaoElementos[y][i]);
						}
						vector <int> vetResp = removerVetor(tam, y);
						recuperarCaminho(vetAux[0], vetResp, tam - 1);
						fprintf(arqSaida, "%d ", vetAux[0]);
						break;
					}
				}
			}
		}
	}
};

int main() {
	TSPD* tspd = new TSPD();
	tspd->algoritmoTSPD();
	delete tspd;
	return 0;
}
