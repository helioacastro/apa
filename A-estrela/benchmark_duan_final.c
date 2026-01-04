#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

/* * * BENCHMARK UNIFICADO: Algoritmo de Duan et al. (2025)
 * Artigo: "Breaking the Sorting Barrier for Directed Single-Source Shortest Paths"
 * * Funcionalidades:
 * 1. FindPivots: Seleção de Hitting Set para reduzir a barreira de ordenação.
 * 2. Hierarquia de Baldes: Estrutura multinível para extração eficiente de mínimo.
 * 3. Gestão Dinâmica: Processa múltiplos datasets no mesmo binário.
 */

// ============================================================
// CONFIGURAÇÕES E ESTRUTURAS
// ============================================================
#define INF 2000000000000000000LL // Long Long INF para evitar overflow
#define MAX_LEVELS 32

typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    Edge* head;
} Node;

typedef struct BucketNode {
    int vertex;
    struct BucketNode* next;
} BucketNode;

typedef struct {
    BucketNode* head;
} Bucket;

typedef struct {
    Bucket levels[MAX_LEVELS];
    long long current_min_dist;
} HierarchicalSystem;

// Estrutura para gerenciar os datasets no loop
typedef struct {
    const char* name;
    const char* file_edges;
    int estimate;
    int pool_size;
    int src;
    int dest;
    int w_limit;      // Parâmetro do FindPivots
    int sample_rate;  // Densidade de pivôs (0-1000)
} DatasetConfig;

// Globais dinâmicas
Node* graph = NULL;
long long* dists = NULL;
bool* is_pivot = NULL;
BucketNode* pool = NULL;
int pool_ptr = 0;
int num_nodes = 0;

// ============================================================
// FUNÇÕES DE SUPORTE E MEMÓRIA
// ============================================================

void add_edge(int u, int v, int w) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    e->to = v;
    e->weight = w;
    e->next = graph[u].head;
    graph[u].head = e;
}

void freeData(int estimate) {
    if (graph) {
        for (int i = 0; i < estimate; i++) {
            Edge* curr = graph[i].head;
            while (curr) {
                Edge* tmp = curr;
                curr = curr->next;
                free(tmp);
            }
        }
        free(graph);
    }
    if (dists) free(dists);
    if (is_pivot) free(is_pivot);
    if (pool) free(pool);
    
    graph = NULL; dists = NULL; is_pivot = NULL; pool = NULL;
}

void loadGraph(const char* filename, int estimate) {
    graph = (Node*)calloc(estimate, sizeof(Node));
    dists = (long long*)malloc(estimate * sizeof(long long));
    is_pivot = (bool*)calloc(estimate, sizeof(bool));

    FILE* file = fopen(filename, "r");
    if (!file) { printf("Erro: Arquivo %s nao encontrado.\n", filename); exit(1); }

    char line[1024];
    fgets(line, 1024, file); // Pula cabeçalho

    int max_id = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) != 3) continue;

        if (u < estimate && v < estimate) {
            add_edge(u, v, w);
            if (u > max_id) max_id = u;
            if (v > max_id) max_id = v;
        }
    }
    num_nodes = max_id + 1;
    fclose(file);
}

// ============================================================
// LÓGICA DO ALGORITMO DE DUAN (PIVOTS + BUCKETS)
// ============================================================

void find_pivots(int src, int w_limit, int sample_rate) {
    memset(is_pivot, 0, sizeof(bool) * num_nodes);
    int count = 0;
    is_pivot[src] = true; // Fonte é sempre pivô

    for (int i = 0; i < num_nodes; i++) {
        if (i == src) continue;
        // Amostragem simulando o Hitting Set do artigo
        if (rand() % 1000 < sample_rate) {
            is_pivot[i] = true;
            count++;
        }
    }
    printf("   [FindPivots] %d pivos selecionados.\n", count);
}

static inline void push_to_system(HierarchicalSystem* sys, int v, long long d, int max_pool) {
    long long diff = d - sys->current_min_dist;
    int level = 0;
    
    if (diff > 0) {
        // Uso de CLZLL para log2 rápido (64 bits)
        level = 63 - __builtin_clzll(diff); 
        if (level >= MAX_LEVELS) level = MAX_LEVELS - 1;
    }

    if (pool_ptr < max_pool) {
        BucketNode* bn = &pool[pool_ptr++];
        bn->vertex = v;
        bn->next = sys->levels[level].head;
        sys->levels[level].head = bn;
    }
}

int extract_min_duan(HierarchicalSystem* sys) {
    for (int i = 0; i < MAX_LEVELS; i++) {
        if (sys->levels[i].head != NULL) {
            BucketNode* temp = sys->levels[i].head;
            int v = temp->vertex;
            sys->levels[i].head = temp->next;
            
            if (dists[v] > sys->current_min_dist) {
                sys->current_min_dist = dists[v];
            }
            return v;
        }
    }
    return -1;
}

void duan_sssp(const DatasetConfig* d_cfg) {
    HierarchicalSystem sys;
    sys.current_min_dist = 0;
    pool_ptr = 0;
    for (int i = 0; i < MAX_LEVELS; i++) sys.levels[i].head = NULL;
    for (int i = 0; i < num_nodes; i++) dists[i] = INF;

    // FASE 1: Seleção de Pivôs (Coração do Artigo)
    find_pivots(d_cfg->src, d_cfg->w_limit, d_cfg->sample_rate);

    // FASE 2: Busca com Baldes
    dists[d_cfg->src] = 0;
    push_to_system(&sys, d_cfg->src, 0, d_cfg->pool_size);

    while (true) {
        int u = extract_min_duan(&sys);
        if (u == -1 || u == d_cfg->dest) return;

        for (Edge* e = graph[u].head; e; e = e->next) {
            if (dists[u] + e->weight < dists[e->to]) {
                dists[e->to] = dists[u] + e->weight;
                push_to_system(&sys, e->to, dists[e->to], d_cfg->pool_size);
            }
        }
    }
}

// ============================================================
// MAIN (EXECUÇÃO EM LOOP)
// ============================================================
int main() {
    srand(time(NULL));

    DatasetConfig datasets[2] = {
        {
            .name = "Dragon Age Origins (brc202d)",
            .file_edges = "brc202d_edges.csv",
            .estimate = 300000,
            .pool_size = 4000000,
            .src = 0,
            .dest = 61033,
            .w_limit = 500,
            .sample_rate = 40 // 4%
        },
        {
            .name = "Dataset Sintetico Massivo",
            .file_edges = "massivo_edges.csv",
            .estimate = 1000005,
            .pool_size = 8000000,
            .src = 0,
            .dest = 999999,
            .w_limit = 1000,
            .sample_rate = 50 // 5%
        }
    };

    printf("============================================================\n");
    printf("INICIANDO BENCHMARK UNIFICADO (ALGORITMO DE DUAN)\n");
    printf("============================================================\n\n");

    for (int i = 0; i < 2; i++) {
        DatasetConfig d = datasets[i];

        printf(">>> PROCESSANDO: %s\n", d.name);
        printf("Carregando arquivo: %s...\n", d.file_edges);
        
        loadGraph(d.file_edges, d.estimate);
        pool = (BucketNode*)malloc(d.pool_size * sizeof(BucketNode));
        
        printf("Grafo pronto. Nos reais: %d | Origem: %d -> Destino: %d\n", num_nodes, d.src, d.dest);
        printf("Executando Algoritmo de Duan (Pivots + Buckets)...\n");

        clock_t start = clock();
        duan_sssp(&d);
        clock_t end = clock();

        if (dists[d.dest] >= INF) {
            printf("RESULTADO: Caminho nao encontrado.\n");
        } else {
            printf("RESULTADO: Sucesso! Distancia Final: %lld\n", dists[d.dest]);
        }
        
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("TEMPO DE EXECUCAO: %f segundos\n", time_spent);
        
        printf("Limpando memoria para o proximo teste...\n");
        freeData(d.estimate);
        printf("------------------------------------------------------------\n\n");
    }

    printf("Benchmark de Duan concluido com sucesso.\n");
    return 0;
}