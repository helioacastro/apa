#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CITIES 20
#define INF INT_MAX

// Estrutura para representar o Grafo (Matriz de Adjacência para simplicidade neste exemplo)
// Em sistemas maiores, Lista de Adjacência é preferível.
typedef struct {
    int num_cidades;
    char nomes[MAX_CITIES][50];
    int distancias[MAX_CITIES][MAX_CITIES];
} MapaRodoviario;

// Função para inicializar o mapa
void inicializarMapa(MapaRodoviario* mapa) {
    mapa->num_cidades = 0;
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            mapa->distancias[i][j] = (i == j) ? 0 : INF;
        }
    }
}

// Adiciona uma cidade ao mapa
int adicionarCidade(MapaRodoviario* mapa, const char* nome) {
    if (mapa->num_cidades >= MAX_CITIES) return -1;
    strcpy(mapa->nomes[mapa->num_cidades], nome);
    return mapa->num_cidades++;
}

// Adiciona uma estrada (aresta) bidirecional entre duas cidades
void adicionarEstrada(MapaRodoviario* mapa, int id_origem, int id_destino, int km) {
    mapa->distancias[id_origem][id_destino] = km;
    mapa->distancias[id_destino][id_origem] = km; // Grafo não-direcionado (estrada de mão dupla)
}

// Função auxiliar para encontrar a cidade com a menor distância não visitada
int minDistance(int dist[], bool visitado[], int num_cidades) {
    int min = INF, min_index;
    for (int v = 0; v < num_cidades; v++)
        if (visitado[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
    return min_index;
}

// Função recursiva para imprimir o caminho
void printPath(int parent[], int j, MapaRodoviario* mapa) {
    if (parent[j] == -1) {
        printf("%s", mapa->nomes[j]);
        return;
    }
    printPath(parent, parent[j], mapa);
    printf(" -> %s", mapa->nomes[j]);
}

// Algoritmo de Dijkstra para encontrar e mostrar a rota
void calcularRota(MapaRodoviario* mapa, int id_origem, int id_destino) {
    int dist[MAX_CITIES];
    bool visitado[MAX_CITIES];
    int parent[MAX_CITIES]; // Armazena a árvore de caminho mínimo

    // Inicialização
    for (int i = 0; i < mapa->num_cidades; i++) {
        dist[i] = INF;
        visitado[i] = false;
        parent[i] = -1;
    }

    dist[id_origem] = 0;

    // Loop principal do Dijkstra
    for (int count = 0; count < mapa->num_cidades - 1; count++) {
        int u = minDistance(dist, visitado, mapa->num_cidades);
        
        // Se a distância for infinita, não há mais alcançáveis
        if (u == INF) break; 
        
        visitado[u] = true;
        
        // Parar se já chegamos ao destino (otimização para ponto-a-ponto)
        if (u == id_destino) break;

        for (int v = 0; v < mapa->num_cidades; v++) {
            if (!visitado[v] && mapa->distancias[u][v] != INF && 
                dist[u] != INF && dist[u] + mapa->distancias[u][v] < dist[v]) {
                dist[v] = dist[u] + mapa->distancias[u][v];
                parent[v] = u; // Rastreia de onde viemos
            }
        }
    }

    // Resultado
    printf("\n--- Resultado da Navegação ---\n");
    if (dist[id_destino] == INF) {
        printf("Não existe rota rodoviária entre %s e %s.\n", 
               mapa->nomes[id_origem], mapa->nomes[id_destino]);
    } else {
        printf("Origem:  %s\n", mapa->nomes[id_origem]);
        printf("Destino: %s\n", mapa->nomes[id_destino]);
        printf("Distância Total: %d km\n", dist[id_destino]);
        printf("Melhor Rota: ");
        printPath(parent, id_destino, mapa);
        printf("\n------------------------------\n");
    }
}

int main() {
    MapaRodoviario mapa;
    inicializarMapa(&mapa);

    // 1. Criando as Cidades (Vértices)
    int sp = adicionarCidade(&mapa, "São Paulo");
    int rj = adicionarCidade(&mapa, "Rio de Janeiro");
    int bh = adicionarCidade(&mapa, "Belo Horizonte");
    int campinas = adicionarCidade(&mapa, "Campinas");
    int santos = adicionarCidade(&mapa, "Santos");
    int curitiba = adicionarCidade(&mapa, "Curitiba");
    int brasilia = adicionarCidade(&mapa, "Brasília");

    // 2. Criando as Estradas (Arestas com Pesos em km)
    // Distâncias aproximadas para exemplo
    adicionarEstrada(&mapa, sp, rj, 430);      // Via Dutra
    adicionarEstrada(&mapa, sp, campinas, 90);
    adicionarEstrada(&mapa, sp, santos, 70);
    adicionarEstrada(&mapa, sp, curitiba, 400);
    adicionarEstrada(&mapa, sp, bh, 580);      // Fernão Dias
    
    adicionarEstrada(&mapa, rj, bh, 440);      // BR-040
    adicionarEstrada(&mapa, bh, brasilia, 740);
    adicionarEstrada(&mapa, campinas, bh, 500); // Rota alternativa
    adicionarEstrada(&mapa, campinas, brasilia, 850);

    // 3. Calculando Rotas
    
    // Exemplo A: São Paulo -> Brasília
    // O algoritmo deve decidir se vai via BH ou Campinas baseado na soma das distâncias
    calcularRota(&mapa, sp, brasilia);

    // Exemplo B: Santos -> Rio de Janeiro
    // Rota: Santos -> SP -> Rio (assumindo que não há Rio-Santos direta cadastrada ou é mais lenta)
    calcularRota(&mapa, santos, rj);

    return 0;
}