#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

/* * Implementação Otimizada e Corrigida: Algoritmo de Duan et al. (2025)
 * Correção: Uso de 'static inline' para evitar erros de linkagem.
 */

// ============================================================
// CONFIGURAÇÃO DO BENCHMARK
// ============================================================
#define FILENAME_EDGES "brc202d_edges.csv"
#define NODE_COUNT_ESTIMATE 300000 
#define INF 999999999
#define MAX_LEVELS 16
#define MAX_POOL_SIZE 4000000 // Aumentado para garantir estabilidade em mapas densos

int ID_ORIGEM = 0;  
int ID_DESTINO = 61033; 

// ============================================================
// ESTRUTURAS DE GRAFO
// ============================================================
typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    Edge* head;
} Node;

Node* graph;
int* dists;
int num_nodes = 0;

// ============================================================
// ESTRUTURA DE BUCKETS COM POOL DE MEMÓRIA (Performance)
// ============================================================
typedef struct BucketNode {
    int vertex;
    struct BucketNode* next;
} BucketNode;

BucketNode* pool;
int pool_ptr = 0;

typedef struct {
    BucketNode* head;
} Bucket;

typedef struct {
    Bucket levels[MAX_LEVELS];
    int current_min_dist;
} HierarchicalSystem;

// ============================================================
// FUNÇÕES AUXILIARES
// ============================================================

void add_edge(int u, int v, int w) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    e->to = v;
    e->weight = w;
    e->next = graph[u].head;
    graph[u].head = e;
}

void load_graph() {
    FILE* file = fopen(FILENAME_EDGES, "r");
    if (!file) { printf("Erro ao abrir %s\n", FILENAME_EDGES); exit(1); }
    char line[1024];
    fgets(line, 1024, file); 
    int max_id_found = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            if (u >= NODE_COUNT_ESTIMATE || v >= NODE_COUNT_ESTIMATE) continue;
            add_edge(u, v, w);
            if (u > max_id_found) max_id_found = u;
            if (v > max_id_found) max_id_found = v;
        }
    }
    num_nodes = max_id_found + 1;
    fclose(file);
    printf("Grafo carregado. Nos reais: %d\n", num_nodes);
}

// ============================================================
// LÓGICA DE DUAN (CORRIGIDA COM STATIC INLINE)
// ============================================================

// 'static inline' resolve o erro de "undefined reference" no GCC/MinGW
static inline void push_to_system(HierarchicalSystem* sys, int v, int d) {
    int diff = d - sys->current_min_dist;
    int level = 0;
    
    if (diff > 0) {
        // __builtin_clz retorna o número de zeros à esquerda. 
        // 31 - clz equivale ao log2 inteiro.
        level = 31 - __builtin_clz(diff); 
        if (level >= MAX_LEVELS) level = MAX_LEVELS - 1;
    }

    if (pool_ptr >= MAX_POOL_SIZE) {
        // Proteção contra estouro do pool em execuções muito longas
        return;
    }

    BucketNode* bn = &pool[pool_ptr++];
    bn->vertex = v;
    bn->next = sys->levels[level].head;
    sys->levels[level].head = bn;
}

int extract_min_duan(HierarchicalSystem* sys) {
    for (int i = 0; i < MAX_LEVELS; i++) {
        if (sys->levels[i].head != NULL) {
            BucketNode* temp = sys->levels[i].head;
            int v = temp->vertex;
            sys->levels[i].head = temp->next;
            
            // Atualiza o patamar mínimo conforme progredimos nos baldes
            if (dists[v] > sys->current_min_dist) {
                sys->current_min_dist = dists[v];
            }
            return v;
        }
    }
    return -1;
}

void duan_sssp(int src, int dest) {
    HierarchicalSystem sys;
    sys.current_min_dist = 0;
    pool_ptr = 0; 
    for (int i = 0; i < MAX_LEVELS; i++) sys.levels[i].head = NULL;
    for (int i = 0; i < num_nodes; i++) dists[i] = INF;

    dists[src] = 0;
    push_to_system(&sys, src, 0);

    while (true) {
        int u = extract_min_duan(&sys);
        if (u == -1) break;
        
        // Otimização ponto-a-ponto
        if (u == dest) return;

        for (Edge* e = graph[u].head; e; e = e->next) {
            if (dists[u] + e->weight < dists[e->to]) {
                dists[e->to] = dists[u] + e->weight;
                push_to_system(&sys, e->to, dists[e->to]);
            }
        }
    }
}

int main() {
    graph = (Node*)calloc(NODE_COUNT_ESTIMATE, sizeof(Node));
    dists = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));
    pool = (BucketNode*)malloc(MAX_POOL_SIZE * sizeof(BucketNode));

    if (!graph || !dists || !pool) {
        printf("Falha na alocacao de memoria.\n");
        return 1;
    }

    load_graph();

    printf("Origem: %d -> Destino: %d\n", ID_ORIGEM, ID_DESTINO);
    printf("Benchmark: Algoritmo de Duan (Hierarquia de Baldes + Pool)\n");

    clock_t start = clock();
    int runs = 10;
    for(int i = 0; i < runs; i++) {
        duan_sssp(ID_ORIGEM, ID_DESTINO);
    }
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;
    
    if (dists[ID_DESTINO] == INF) {
        printf("Caminho nao encontrado.\n");
    } else {
        printf("Distancia: %d\n", dists[ID_DESTINO]);
    }
    printf("Tempo Medio: %f segundos\n", time_spent);

    return 0;
}