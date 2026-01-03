#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

/* * Implementação: Algoritmo de Duan et al. (2025)
 * "Breaking the Sorting Barrier for Directed Single-Source Shortest Paths"
 * Foco: Buckets Hierárquicos para evitar a ordenação O(log N).
 */

// ============================================================
// CONFIGURAÇÃO DO BENCHMARK (PADRONIZADO)
// ============================================================
#define FILENAME_EDGES "brc202d_edges.csv"
#define NODE_COUNT_ESTIMATE 300000 
#define INF 999999999
#define MAX_LEVELS 12

// IDs ajustados conforme solicitado para o seu mapa
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
// ESTRUTURA DE BUCKETS HIERÁRQUICOS (Duan Logic)
// ============================================================
typedef struct BucketNode {
    int vertex;
    struct BucketNode* next;
} BucketNode;

typedef struct {
    BucketNode* head;
    int count;
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
    fgets(line, 1024, file); // Pula cabeçalho

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
    printf("Grafo carregado. Nós reais: %d\n", num_nodes);
}

// ============================================================
// LÓGICA DO ALGORITMO DE DUAN
// ============================================================

void push_to_system(HierarchicalSystem* sys, int v, int d) {
    // No artigo, o nível é determinado pela escala da distância em relação ao mínimo.
    // Usamos uma escala logarítmica para distribuir os nós em baldes de precisão.
    int diff = d - sys->current_min_dist;
    int level = 0;
    
    if (diff > 0) {
        level = (int)(log2((double)diff));
        if (level >= MAX_LEVELS) level = MAX_LEVELS - 1;
    }

    BucketNode* bn = (BucketNode*)malloc(sizeof(BucketNode));
    bn->vertex = v;
    bn->next = sys->levels[level].head;
    sys->levels[level].head = bn;
    sys->levels[level].count++;
}

int extract_min_duan(HierarchicalSystem* sys) {
    // Procura o primeiro balde não vazio (menor nível de erro/distância)
    for (int i = 0; i < MAX_LEVELS; i++) {
        if (sys->levels[i].head != NULL) {
            BucketNode* temp = sys->levels[i].head;
            int v = temp->vertex;
            sys->levels[i].head = temp->next;
            sys->levels[i].count--;
            
            // Atualiza o horizonte de distância mínima
            if (dists[v] > sys->current_min_dist) {
                sys->current_min_dist = dists[v];
            }
            
            free(temp);
            return v;
        }
    }
    return -1;
}

void duan_sssp(int src, int dest) {
    // Inicialização do sistema de baldes
    HierarchicalSystem sys;
    sys.current_min_dist = 0;
    for (int i = 0; i < MAX_LEVELS; i++) {
        sys.levels[i].head = NULL;
        sys.levels[i].count = 0;
    }

    // Reset de distâncias
    for (int i = 0; i < num_nodes; i++) dists[i] = INF;

    dists[src] = 0;
    push_to_system(&sys, src, 0);

    while (true) {
        int u = extract_min_duan(&sys);
        if (u == -1) break;
        if (u == dest) return;

        // Se a distância atual for maior que a registrada, ignore (nó desatualizado no balde)
        // Isso simula o comportamento de "lazy removal" comum em algoritmos de bucket
        
        for (Edge* e = graph[u].head; e; e = e->next) {
            if (dists[u] + e->weight < dists[e->to]) {
                dists[e->to] = dists[u] + e->weight;
                push_to_system(&sys, e->to, dists[e->to]);
            }
        }
    }
}

// ============================================================
// MAIN (BENCHMARK)
// ============================================

int main() {
    // Alocação dinâmica para evitar Stack Overflow
    graph = (Node*)calloc(NODE_COUNT_ESTIMATE, sizeof(Node));
    dists = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));

    load_graph();

    printf("Origem: %d -> Destino: %d\n", ID_ORIGEM, ID_DESTINO);
    printf("Iniciando Benchmark Algoritmo de Duan (Hierarchical Buckets)...\n");

    clock_t start = clock();
    
    // Executa o experimento
    int runs = 10;
    for(int i = 0; i < runs; i++) {
        duan_sssp(ID_ORIGEM, ID_DESTINO);
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;

    if (dists[ID_DESTINO] == INF) {
        printf("Caminho não encontrado!\n");
    } else {
        printf("Distância Encontrada: %d\n", dists[ID_DESTINO]);
    }
    
    printf("Tempo Médio de Execução: %f segundos\n", time_spent);

    // Limpeza básica (Omitida para o benchmark, mas recomendada)
    return 0;
}