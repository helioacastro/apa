#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

// ============================================================
// ESTRUTURAS E CONFIGURAÇÕES
// ============================================================
#define INF 2000000000000000000LL // Long Long INF para evitar overflow no massivo

typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    Edge* head;
} Node;

typedef struct {
    int vertex;
    long long dist;
} HeapNode;

// Estrutura para gerenciar os datasets no loop
typedef struct {
    const char* name;
    const char* file_edges;
    int estimate;
    int src;
    int dest;
} DatasetConfig;

// Globais dinâmicas
Node* graph = NULL;
long long* dist = NULL;
HeapNode* minHeap = NULL;
int* pos = NULL;
int num_nodes = 0;
int heapSize = 0;

// ============================================================
// FUNÇÕES DE SUPORTE (MIN-HEAP)
// ============================================================

void swap(HeapNode* a, HeapNode* b) {
    HeapNode t = *a; *a = *b; *b = t;
}

void minHeapify(int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heapSize && minHeap[left].dist < minHeap[smallest].dist)
        smallest = left;
    if (right < heapSize && minHeap[right].dist < minHeap[smallest].dist)
        smallest = right;

    if (smallest != idx) {
        pos[minHeap[smallest].vertex] = idx;
        pos[minHeap[idx].vertex] = smallest;
        swap(&minHeap[smallest], &minHeap[idx]);
        minHeapify(smallest);
    }
}

HeapNode extractMin() {
    HeapNode root = minHeap[0];
    HeapNode last = minHeap[heapSize - 1];
    minHeap[0] = last;
    pos[root.vertex] = heapSize - 1;
    pos[last.vertex] = 0;
    heapSize--;
    minHeapify(0);
    return root;
}

void decreaseKey(int v, long long d) {
    int i = pos[v];
    minHeap[i].dist = d;
    while (i && minHeap[i].dist < minHeap[(i - 1) / 2].dist) {
        pos[minHeap[i].vertex] = (i - 1) / 2;
        pos[minHeap[(i - 1) / 2].vertex] = i;
        swap(&minHeap[i], &minHeap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

bool isInMinHeap(int v) {
    return pos[v] < heapSize;
}

// ============================================================
// GESTÃO DE MEMÓRIA E CARREGAMENTO
// ============================================================

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
    if (dist) free(dist);
    if (minHeap) free(minHeap);
    if (pos) free(pos);
    
    graph = NULL; dist = NULL; minHeap = NULL; pos = NULL;
}

void loadGraph(const char* filename, int estimate) {
    // Alocação dinâmica no Heap
    graph = (Node*)calloc(estimate, sizeof(Node));
    dist = (long long*)malloc(estimate * sizeof(long long));
    minHeap = (HeapNode*)malloc(estimate * sizeof(HeapNode));
    pos = (int*)malloc(estimate * sizeof(int));

    if (!graph || !dist || !minHeap || !pos) {
        printf("Erro fatal: Falha na alocacao de memoria.\n");
        exit(1);
    }

    FILE* file = fopen(filename, "r");
    if (!file) { printf("Erro: Arquivo %s nao encontrado.\n", filename); exit(1); }

    char line[1024];
    fgets(line, 1024, file); // Pula cabeçalho

    int max_id = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) != 3) continue;

        if (u < estimate && v < estimate) {
            Edge* newEdge = (Edge*)malloc(sizeof(Edge));
            newEdge->to = v;
            newEdge->weight = w;
            newEdge->next = graph[u].head;
            graph[u].head = newEdge;

            if (u > max_id) max_id = u;
            if (v > max_id) max_id = v;
        }
    }
    num_nodes = max_id + 1;
    fclose(file);
}

// ============================================================
// ALGORITMO DIJKSTRA
// ============================================================
void dijkstra(int src, int dest) {
    for (int v = 0; v < num_nodes; ++v) {
        dist[v] = INF;
        minHeap[v].vertex = v;
        minHeap[v].dist = INF;
        pos[v] = v;
    }

    dist[src] = 0;
    heapSize = num_nodes;
    decreaseKey(src, 0);

    while (heapSize != 0) {
        HeapNode minNode = extractMin();
        int u = minNode.vertex;

        if (u == dest) return; // Otimização: paramos ao encontrar o destino
        if (dist[u] == INF) break;

        Edge* crawl = graph[u].head;
        while (crawl != NULL) {
            int v = crawl->to;
            if (isInMinHeap(v) && dist[u] + crawl->weight < dist[v]) {
                dist[v] = dist[u] + crawl->weight;
                decreaseKey(v, dist[v]);
            }
            crawl = crawl->next;
        }
    }
}

// ============================================================
// MAIN (EXECUÇÃO SEQUENCIAL)
// ============================================================
int main() {
    DatasetConfig datasets[2] = {
        {
            .name = "Dragon Age Origins (brc202d)",
            .file_edges = "brc202d_edges.csv",
            .estimate = 300000,
            .src = 0,
            .dest = 61033
        },
        {
            .name = "Dataset Sintetico Massivo",
            .file_edges = "massivo_edges.csv",
            .estimate = 1000005,
            .src = 0,
            .dest = 999999
        }
    };

    printf("============================================================\n");
    printf("INICIANDO BENCHMARK UNIFICADO (DIJKSTRA)\n");
    printf("============================================================\n\n");

    for (int i = 0; i < 2; i++) {
        DatasetConfig d = datasets[i];

        printf(">>> PROCESSANDO: %s\n", d.name);
        printf("Carregando arquivo: %s...\n", d.file_edges);
        
        loadGraph(d.file_edges, d.estimate);
        
        printf("Grafo carregado. Nos reais: %d | Origem: %d -> Destino: %d\n", num_nodes, d.src, d.dest);
        printf("Executando busca Dijkstra...\n");

        clock_t start = clock();
        dijkstra(d.src, d.dest);
        clock_t end = clock();

        if (dist[d.dest] >= INF) {
            printf("RESULTADO: Caminho nao encontrado.\n");
        } else {
            printf("RESULTADO: Sucesso! Distancia Total: %lld\n", dist[d.dest]);
        }
        
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("TEMPO DE EXECUCAO: %f segundos\n", time_spent);
        
        printf("Limpando memoria para o proximo teste...\n");
        freeData(d.estimate);
        printf("------------------------------------------------------------\n\n");
    }

    printf("Benchmark de Dijkstra concluido com sucesso.\n");
    return 0;
}