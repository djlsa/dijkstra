/*
 ============================================================================
 Name        : dijsp.c
 Author      : David Salsinha
 ============================================================================
 */

#include "dijsp.h"

#include <stdio.h>
#include <stdlib.h>

/* "Construtor do Grafo */
Grafo* Grafo_init(int nVertices, int nArestas) {
	int i;
	Grafo* g = malloc(sizeof(Grafo));

	g->nVertices = nVertices;
	g->nArestas = nArestas;
	g->pesos = malloc(nVertices * sizeof(int*));
	for (i = 0; i < nVertices; i++) {
		/* Inicializa o array de cada vertice origem com peso zero para todos
		 * os destinos
		 */
		g->pesos[i] = calloc(nVertices, sizeof(int));
	}
	return g;
}

/* "Destrutor" do Grafo */
void Grafo_free(Grafo* g) {
	int i;

	for (i = 0; i < g->nVertices; i++)
		free(g->pesos[i]);
	free(g->pesos);
	free(g);
}

/* Função auxiliar para efetuar a leitura de valores a partir de uma linha */
int _Grafo_lerDeFicheiro_lerValores(FILE* f, Grafo_FormatoLeitura formato,
		int* valores) {
	char linha[SHRT_MAX], c;

	/* Ignorar comentários e linhas em branco */
	while (fgets(linha, SHRT_MAX, f) != NULL) {
		sscanf(linha, "%c", &c);
		if (c == '#' || c == '\r' || c == '\n') {
			continue;
		}
		break;
	}

	/* Tentar efetuar a leitura dos valores para o array */
	switch (formato) {
	case FORMATO_NVERTICES:
		return sscanf(linha, "N=%d", &valores[0]);
	case FORMATO_NARESTAS:
		return sscanf(linha, "E=%d", &valores[0]);
	case FORMATO_ARESTA:
		return sscanf(linha, "%d->%d %d", &valores[0], &valores[1],
				&valores[2]);
	}
	return EOF;
}

/* Mostrar erro ocorrido na leitura do ficheiro */
void _Grafo_lerDeFicheiro_erro(Grafo_ErroLeitura resultado) {
	switch (resultado) {
	case ERRO_FICHEIRO:
		printf(MSG_ERRO_FICHEIRO);
		break;
	case ERRO_FORMATO:
		printf(MSG_ERRO_FORMATO);
		break;
	}
}

/* Leitura de um Grafo a partir de ficheiro */
Grafo* Grafo_lerDeFicheiro(char* nomeFicheiro) {
	Grafo* g;
	int nVertices[1], nArestas[1], pesoAresta[3];
	int i;
	FILE* f = fopen(nomeFicheiro, "r");

	/* Ficheiro não existente ou outro erro desse tipo */
	if (f == NULL) {
		_Grafo_lerDeFicheiro_erro(ERRO_FICHEIRO);
		return NULL;
	}

	/* Tentar ler o numero de vertices */
	if (_Grafo_lerDeFicheiro_lerValores(f, FORMATO_NVERTICES, nVertices) != 1) {
		_Grafo_lerDeFicheiro_erro(ERRO_FORMATO);
		return NULL;
	}

	/* Tentar ler o numero de arestas */
	if (_Grafo_lerDeFicheiro_lerValores(f, FORMATO_NARESTAS, nArestas) != 1) {
		_Grafo_lerDeFicheiro_erro(ERRO_FORMATO);
		return NULL;
	}

	/* Inicializar o grafo */
	g = Grafo_init(nVertices[0], nArestas[0]);

	/* Efetuar a leitura das arestas */
	for (i = 0; i < nArestas[0]; i++) {
		if (_Grafo_lerDeFicheiro_lerValores(f, FORMATO_ARESTA, pesoAresta) != 3
				&& pesoAresta[0] >= 0 && pesoAresta[0] < g->nVertices
				&& pesoAresta[1] >= 0 && pesoAresta[1] < g->nVertices) {

			_Grafo_lerDeFicheiro_erro(ERRO_FORMATO);
			return NULL;

		}
		/* pesos[origem][destino] = p */
		g->pesos[pesoAresta[0]][pesoAresta[1]] = pesoAresta[2];
	}
	return g;
}

/* Inicialização da estrutura usada no algoritmo de Dijsktra */
Dijsktra* Dijsktra_init(Grafo* g, int verticeInicial) {
	int i;
	Dijsktra* d = malloc(sizeof(Dijsktra));

	d->grafo = g;
	d->verticeInicial = verticeInicial;
	d->caminhos = malloc(g->nVertices * sizeof(Dijsktra_Caminho*));
	for (i = 0; i < g->nVertices; i++) {
		d->caminhos[i] = malloc(sizeof(Dijsktra_Caminho));
		/* Inicializa o array de caminhos com peso infinito, que será o valor
		 * final para o caso dos vertices nao acessiveis a partir do inicial
		 */
		d->caminhos[i]->peso = INFINITO;
		d->caminhos[i]->nArestas = 0;
	}
	d->caminhos[verticeInicial]->peso = 0; /* Inicial tem distancia zero */
	d->jaPercorrido = calloc(g->nVertices, sizeof(bool));
	return d;
}

/* "Destrutor" */
void Dijsktra_free(Dijsktra* d) {
	int i;

	free(d->jaPercorrido);
	for (i = 0; i < d->grafo->nVertices; i++)
		free(d->caminhos[i]);
	free(d->caminhos);
	free(d);
}

/* Função auxiliar que retorna o indice do vertice com distancia minima */
int _Dijsktra_caminhosMaisCurtos_indiceMinimaDistancia(Dijsktra* d) {
	int minimaDistancia = INFINITO, indice, i;

	for (i = 0; i < d->grafo->nVertices; i++) {
		/* Verifica-se se o vertice ainda nao foi percorrido */
		if (!d->jaPercorrido[i] && d->caminhos[i]->peso <= minimaDistancia) {
			minimaDistancia = d->caminhos[i]->peso;
			indice = i;
		}
	}
	return indice;
}

/* Função que retorna um array com os caminhos mais curtos */
Dijsktra_Caminho** Dijsktra_caminhosMaisCurtos(Dijsktra* d) {
	int i, j, indiceMinPeso, nVertices = d->grafo->nVertices;
	int** grafo = d->grafo->pesos; /* "atalho" para o grafo */
	Dijsktra_Caminho** caminhos = d->caminhos; /* "atalho" para os caminhos */

	/* Percorrem-se todos os vertices destino */
	for (i = 0; i < nVertices; i++) {
		/* Indice do vertice com menor peso */
		indiceMinPeso = _Dijsktra_caminhosMaisCurtos_indiceMinimaDistancia(d);
		/* Marca-se o vertice como ja percorrido */
		d->jaPercorrido[indiceMinPeso] = true;
		/* Percorrem-se todos os vertices j*/
		for (j = 0; j < nVertices; j++) {
			/* Algoritmo de Dijsktra */
			if (!d->jaPercorrido[j] && grafo[indiceMinPeso][j] != 0
				&& caminhos[indiceMinPeso]->peso != INFINITO
				&& caminhos[indiceMinPeso]->peso + grafo[indiceMinPeso][j] < caminhos[j]->peso) {

				/* Distancia para o vertice j */
				caminhos[j]->peso = caminhos[indiceMinPeso]->peso + grafo[indiceMinPeso][j];
				/* Numero de arestas percorridas até ao vertice */
				caminhos[j]->nArestas = caminhos[indiceMinPeso]->nArestas + 1;

			}
		}
	}
	return caminhos;
}

/* Função para mostrar os resultados em output */
void Dijsktra_printCaminhosMaisCurtos(Dijsktra_Caminho** caminhos,
		int nVertices) {
	int i, nCaminhos = 0;

	/* Verificar quais são caminhos validos */
	for (i = 0; i < nVertices; i++) {
		/* Peso 0 = vertice inicial, INFINITO = inacessivel */
		if (caminhos[i]->peso != 0 && caminhos[i]->peso != INFINITO)
			nCaminhos++;
	}
	printf("Caminhos mais curtos encontrados= %d\n\n", nCaminhos);

	printf("Destino | Distancia | Num. arestas caminho\n");

	for (i = 0; i < nVertices; i++) {
		/* Não mostrar os dados do vertice inicial */
		if (caminhos[i]->peso == 0)
			continue;

		/* Indice do vertice destino */
		printf("%5d    ", i);

		if (caminhos[i]->peso != INFINITO) {
			/* Distancia e numero de arestas */
			printf("%7d     ", caminhos[i]->peso);
			printf("%9d", caminhos[i]->nArestas);
		} else {
			/* Dados vertice inacessivel */
			printf("    inf             -");
		}
		printf("\n");
	}
}

int main(int argc, char** argv) {
	Grafo* grafo = NULL;
	Dijsktra* dijsktra;
	Dijsktra_Caminho** caminhos;
	int verticeInicial;

	/* Verificar argumentos da linha de comandos */
	if (argc != 3) {
		printf("dijsp  nomeficheiro  verticeinicial");
		return EXIT_FAILURE;
	}

	/* Ler dados do grafo a partir do ficheiro especificado */
	grafo = Grafo_lerDeFicheiro(argv[1]);
	if (grafo == NULL)
		return EXIT_FAILURE;

	/* Vertice inicial */
	verticeInicial = atoi(argv[2]);

	printf("Grafo= %s\n", argv[1]);
	printf("Vertices= %d\n", grafo->nVertices);
	printf("Arestas= %d\n", grafo->nArestas);
	printf("Vertice inicial= %d\n", verticeInicial);

	/* Calculo dos caminhos mais curtos pelo algoritmo de Dijsktra */
	dijsktra = Dijsktra_init(grafo, verticeInicial);
	caminhos = Dijsktra_caminhosMaisCurtos(dijsktra);
	Dijsktra_printCaminhosMaisCurtos(caminhos, grafo->nVertices);

	/* Liberta memoria alocada */
	Dijsktra_free(dijsktra);
	Grafo_free(grafo);

	return EXIT_SUCCESS;
}
