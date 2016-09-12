#include <limits.h>

#define INFINITO	INT_MAX

typedef int		bool;
#define false	0
#define true	1

/* Grafo */
typedef struct {
	int nVertices;
	int nArestas;
	int** pesos; /* array c/ pesos das arestas (peso[origem][destino] = p) */
} Grafo;

typedef enum {
	FORMATO_NVERTICES, FORMATO_NARESTAS, FORMATO_ARESTA
} Grafo_FormatoLeitura;

typedef enum {
	ERRO_FICHEIRO, ERRO_FORMATO
} Grafo_ErroLeitura;

#define MSG_ERRO_FICHEIRO	"Erro ao ler o ficheiro.\n"
#define MSG_ERRO_FORMATO	"Erro no formato do ficheiro.\n"

/* Caminho para um determinado vertice */
typedef struct {
	int peso;		/* distancia total percorrida */
	int nArestas;	/* numero de arestas percorridas */
} Dijsktra_Caminho;

/* Estrutura com variaveis usadas pelo algoritmo de Dijsktra */
typedef struct {
	Grafo* grafo;
	int verticeInicial;
	Dijsktra_Caminho** caminhos; /* caminhos[verticeDestino] = caminho */
	bool* jaPercorrido; /* array para guardar vertices já percorridos */
} Dijsktra;
