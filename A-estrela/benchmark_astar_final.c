#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

// ============================================================
// ESTRUTURAS E CONFIGURAÇÕES
// ============================================================
#define INF 2000000000000000000LL // Long Long INF para segurança

typedef struct {
    double x, y;
} Coord;

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
    long long fScore; 
} HeapNode;

// Globais dinâmicas
Node* graph = NULL;
Coord* nodeCoords = NULL;
long long* gScore = NULL;
HeapNode* minHeap = NULL;
int* pos = NULL;
int num_nodes = 0;
int heapSize = 0;
bool is_massive_mode = false;

// Estrutura para gerenciar os datasets no loop
typedef struct {
    const char* name;
    const char* file_nodes;
    const char* file_edges;
    int estimate;
    int src;
    int dest;
    bool massive;
} DatasetConfig;

// ============================================================
// FUNÇÕES DE SUPORTE (HEAP)
// ============================================================

void swap(HeapNode* a, HeapNode* b) {
    HeapNode t = *a; *a = *b; *b = t;
}

void minHeapify(int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    if (left < heapSize && minHeap[left].fScore < minHeap[smallest].fScore) smallest = left;
    if (right < heapSize && minHeap[right].fScore < minHeap[smallest].fScore) smallest = right;
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

void decreaseKey(int v, long long f) {
    int i = pos[v];
    minHeap[i].fScore = f;
    while (i && minHeap[i].fScore < minHeap[(i - 1) / 2].fScore) {
        pos[minHeap[i].vertex] = (i - 1) / 2;
        pos[minHeap[(i - 1) / 2].vertex] = i;
        swap(&minHeap[i], &minHeap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// ============================================================
// HEURÍSTICA ADAPTATIVA
// ============================================================
long long heuristic(int u, int target) {
    if (is_massive_mode) {
        // Dataset Sintético: 1D (Eixo X) com peso ~1000
        return (long long)fabs(nodeCoords[u].x - nodeCoords[target].x) * 1000;
    } else {
        // Dataset Dragon Age: 2D Euclidiana com peso base 10
        double dx = nodeCoords[u].x - nodeCoords[target].x;
        double dy = nodeCoords[u].y - nodeCoords[target].y;
        return (long long)(sqrt(dx*dx + dy*dy) * 10.0);
    }
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
    if (nodeCoords) free(nodeCoords);
    if (gScore) free(gScore);
    if (minHeap) free(minHeap);
    if (pos) free(pos);
    
    graph = NULL; nodeCoords = NULL; gScore = NULL; minHeap = NULL; pos = NULL;
}

void loadData(const char* file_nodes, const char* file_edges, int estimate) {
    // Alocação dinâmica baseada no estimate
    graph = (Node*)calloc(estimate, sizeof(Node));
    nodeCoords = (Coord*)malloc(estimate * sizeof(Coord));
    gScore = (long long*)malloc(estimate * sizeof(long long));
    minHeap = (HeapNode*)malloc(estimate * sizeof(HeapNode));
    pos = (int*)malloc(estimate * sizeof(int));

    if (!graph || !nodeCoords || !gScore || !minHeap || !pos) {
        printf("Erro fatal: Falha na alocacao de memoria.\n");
        exit(1);
    }

    char line[1024];
    
    // 1. Carregar Nós
    FILE* fNodes = fopen(file_nodes, "r");
    if (!fNodes) { printf("Erro: Arquivo %s nao encontrado.\n", file_nodes); exit(1); }
    fgets(line, 1024, fNodes); // Header
    while (fgets(line, 1024, fNodes)) {
        int id; double x, y;
        if (is_massive_mode) {
            if (sscanf(line, "%d,%*d,%lf,%*lf", &id, &x) == 2) {
                if (id < estimate) { nodeCoords[id].x = x; nodeCoords[id].y = 0; }
            }
        } else {
            if (sscanf(line, "%d,%*d,%lf,%lf", &id, &x, &y) == 3) {
                if (id < estimate) { nodeCoords[id].x = x; nodeCoords[id].y = y; }
            }
        }
    }
    fclose(fNodes);

    // 2. Carregar Arestas
    FILE* fEdges = fopen(file_edges, "r");
    if (!fEdges) { printf("Erro: Arquivo %s nao encontrado.\n", file_edges); exit(1); }
    fgets(line, 1024, fEdges); 
    int max_id = 0;
    while (fgets(line, 1024, fEdges)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            if (u < estimate && v < estimate) {
                Edge* n = malloc(sizeof(Edge));
                n->to = v; n->weight = w; n->next = graph[u].head;
                graph[u].head = n;
                if(u > max_id) max_id = u; if(v > max_id) max_id = v;
            }
        }
    }
    num_nodes = max_id + 1;
    fclose(fEdges);
}

// ============================================================
// ALGORITMO A*
// ============================================================
void a_star(int src, int dest) {
    for (int v = 0; v < num_nodes; ++v) {
        gScore[v] = INF;
        minHeap[v].vertex = v;
        minHeap[v].fScore = INF;
        pos[v] = v;
    }
    gScore[src] = 0;
    heapSize = num_nodes;
    decreaseKey(src, heuristic(src, dest));

    while (heapSize != 0) {
        HeapNode minNode = extractMin();
        int u = minNode.vertex;
        if (u == dest) return;
        if (gScore[u] == INF) break;

        for (Edge* e = graph[u].head; e; e = e->next) {
            long long tent = gScore[u] + e->weight;
            if (pos[e->to] < heapSize && tent < gScore[e->to]) {
                gScore[e->to] = tent;
                decreaseKey(e->to, tent + heuristic(e->to, dest));
            }
        }
    }
}

// ============================================================
// MAIN (PROCESSAMENTO SEQUENCIAL)
// ============================================================
int main() {
    // Definição dos dois datasets para o benchmark
    DatasetConfig datasets[2] = {
        {
            .name = "Dragon Age Origins (brc202d)",
            .file_nodes = "brc202d_nodes.csv",
            .file_edges = "brc202d_edges.csv",
            .estimate = 300000,
            .src = 0,
            .dest = 61033,
            .massive = false
        },
        {
            .name = "Dataset Sintetico Massivo",
            .file_nodes = "massivo_nodes.csv",
            .file_edges = "massivo_edges.csv",
            .estimate = 1000005,
            .src = 0,
            .dest = 999999,
            .massive = true
        }
    };

    printf("============================================================\n");
    printf("INICIANDO BENCHMARK UNIFICADO (A*)\n");
    printf("============================================================\n\n");

    for (int i = 0; i < 2; i++) {
        DatasetConfig d = datasets[i];
        is_massive_mode = d.massive;

        printf(">>> PROCESSANDO: %s\n", d.name);
        printf("Carregando arquivos: %s / %s...\n", d.file_nodes, d.file_edges);
        
        loadData(d.file_nodes, d.file_edges, d.estimate);
        
        printf("Grafo pronto. Nos: %d | Origem: %d -> Destino: %d\n", num_nodes, d.src, d.dest);
        printf("Executando busca A*...\n");

        clock_t start = clock();
        a_star(d.src, d.dest);
        clock_t end = clock();

        if (gScore[d.dest] >= INF) {
            printf("RESULTADO: Caminho nao encontrado.\n");
        } else {
            printf("RESULTADO: Sucesso! Custo Final: %lld\n", gScore[d.dest]);
        }
        
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("TEMPO DE EXECUCAO: %f segundos\n", time_spent);
        
        printf("Limpando memoria para o proximo teste...\n");
        freeData(d.estimate);
        printf("------------------------------------------------------------\n\n");
    }

    printf("Benchmark concluido com sucesso.\n");
    return 0;
}