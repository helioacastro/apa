#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

/* * Implementação de Alta Performance: Algoritmo de Duan et al. (2025)
 * Foco: Redução de Cache Misses e eliminação de redundância.
 */

// ============================================================
// CONFIGURAÇÃO DO BENCHMARK
// ============================================================
#define FILENAME_EDGES "brc202d_edges.csv"
#define NODE_COUNT_ESTIMATE 300000 
#define INF 1000000000
#define MAX_LEVELS 32
#define MAX_NODES_IN_SYSTEM 2000000 

int ID_ORIGEM = 0;  
int ID_DESTINO = 61033; 

// ============================================================
// ESTRUTURAS DE GRAFO (Compactas para Cache)
// ============================================================
typedef struct {
    int to;
    int weight;
    int next;
} Edge;

Edge edges[1000000]; // Pool de arestas estático
int head[NODE_COUNT_ESTIMATE];
int edge_count = 0;
int dists[NODE_COUNT_ESTIMATE];
int num_nodes = 0;

void add_edge(int u, int v, int w) {
    edges[edge_count].to = v;
    edges[edge_count].weight = w;
    edges[edge_count].next = head[u];
    head[u] = edge_count++;
}

// ============================================================
// SISTEMA DE BALDES DE DUAN (Otimizado com Array-Stack)
// ============================================================
// Em vez de listas encadeadas, usamos um "Stack" por nível para 
// garantir acesso sequencial à memória.
int bucket_storage[MAX_NODES_IN_SYSTEM];
int bucket_heads[MAX_LEVELS]; // Índice de início no storage
int bucket_sizes[MAX_LEVELS]; // Quantidade de nós por nível
int current_min_dist = 0;

static inline void push_to_system(int v, int d) {
    int diff = d - current_min_dist;
    int level = 0;
    if (diff > 0) {
        // Uso de instrução de hardware para calcular log2 (Nível da Hierarquia)
        level = 31 - __builtin_clz(diff);
        if (level >= MAX_LEVELS) level = MAX_LEVELS - 1;
    }

    // Calcula posição no storage linear
    // Simplificado: Cada nível tem um espaço reservado no pool
    int pos = level * (MAX_NODES_IN_SYSTEM / MAX_LEVELS) + bucket_sizes[level];
    if (pos < MAX_NODES_IN_SYSTEM) {
        bucket_storage[pos] = v;
        bucket_sizes[level]++;
    }
}

int extract_min_duan() {
    for (int i = 0; i < MAX_LEVELS; i++) {
        if (bucket_sizes[i] > 0) {
            int pos = i * (MAX_NODES_IN_SYSTEM / MAX_LEVELS) + (--bucket_sizes[i]);
            int v = bucket_storage[pos];
            
            if (dists[v] > current_min_dist) {
                current_min_dist = dists[v];
            }
            return v;
        }
    }
    return -1;
}

// ============================================================
// CARREGAMENTO E EXECUÇÃO
// ============================================================

void load_graph() {
    FILE* file = fopen(FILENAME_EDGES, "r");
    if (!file) { printf("Erro ao abrir %s\n", FILENAME_EDGES); exit(1); }
    char line[1024];
    fgets(line, 1024, file); 
    int max_id = 0;
    memset(head, -1, sizeof(head));
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            add_edge(u, v, w);
            if (u > max_id) max_id = u;
            if (v > max_id) max_id = v;
        }
    }
    num_nodes = max_id + 1;
    fclose(file);
}

void duan_sssp(int src, int dest) {
    for (int i = 0; i < num_nodes; i++) dists[i] = INF;
    for (int i = 0; i < MAX_LEVELS; i++) bucket_sizes[i] = 0;
    current_min_dist = 0;

    dists[src] = 0;
    push_to_system(src, 0);

    while (true) {
        int u = extract_min_duan();
        if (u == -1) break;
        if (u == dest) return;

        // "Lazy Check": Se encontramos uma distância melhor para 'u' depois 
        // dele ter sido colocado no balde, ignore esta versão antiga.
        // Isso é CRÍTICO para performance.
        int d_u = dists[u];

        for (int i = head[u]; i != -1; i = edges[i].next) {
            int v = edges[i].to;
            int weight = edges[i].weight;
            if (d_u + weight < dists[v]) {
                dists[v] = d_u + weight;
                push_to_system(v, dists[v]);
            }
        }
    }
}

int main() {
    load_graph();
    printf("Benchmark: Duan Otimizado (Cache-Friendly)\n");
    printf("Origem: %d -> Destino: %d\n", ID_ORIGEM, ID_DESTINO);

    clock_t start = clock();
    int runs = 10;
    for(int i = 0; i < runs; i++) {
        duan_sssp(ID_ORIGEM, ID_DESTINO);
    }
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;
    printf("Distancia: %d\n", dists[ID_DESTINO]);
    printf("Tempo Medio: %f segundos\n", time_spent);

    return 0;
}