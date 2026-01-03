#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

// ============================================================
// CONFIGURAÇÃO PARA DATASET MASSIVO
// ============================================================
#define FILENAME_EDGES "massivo_edges.csv"
#define FILENAME_NODES "massivo_nodes.csv"
#define NODE_COUNT_ESTIMATE 1000005 
#define INF 2000000000LL

int ID_ORIGEM = 0;  
int ID_DESTINO = 999999; 

typedef struct {
    int x; // Dataset sintético posiciona nós linearmente no eixo X
} Coord;

Coord* nodeCoords;

typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    Edge* head;
} Node;

Node* graph;
long long* gScore;
int num_nodes = 0;

typedef struct {
    int vertex;
    long long fScore; 
} HeapNode;

HeapNode* minHeap;
int* pos; 
int heapSize = 0;

// Heurística para o dataset sintético: a distância X multiplicada 
// pelo peso médio aproximado da espinha dorsal (~1000)
long long heuristic(int u, int target) {
    return (long long)abs(nodeCoords[u].x - nodeCoords[target].x) * 1000;
}

// --- Funções de Heap ---
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
// CARREGAMENTO
// ============================================================
void loadData() {
    // 1. Carregar Coordenadas
    FILE* file = fopen(FILENAME_NODES, "r");
    if (!file) { printf("Erro ao abrir nós\n"); exit(1); }
    char line[1024];
    fgets(line, 1024, file);
    while (fgets(line, 1024, file)) {
        int id, osmid, x, y;
        if (sscanf(line, "%d,%d,%d,%d", &id, &osmid, &x, &y) == 4) {
            if (id < NODE_COUNT_ESTIMATE) nodeCoords[id].x = x;
        }
    }
    fclose(file);

    // 2. Carregar Arestas
    file = fopen(FILENAME_EDGES, "r");
    if (!file) { printf("Erro ao abrir arestas\n"); exit(1); }
    fgets(line, 1024, file);
    int max_id = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            if (u >= NODE_COUNT_ESTIMATE || v >= NODE_COUNT_ESTIMATE) continue;
            Edge* n = malloc(sizeof(Edge));
            n->to = v; n->weight = w; n->next = graph[u].head;
            graph[u].head = n;
            if (u > max_id) max_id = u; if (v > max_id) max_id = v;
        }
    }
    num_nodes = max_id + 1;
    fclose(file);
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

        for (Edge* e = graph[u].head; e; e = e->next) {
            int v = e->to;
            long long tentative_g = gScore[u] + e->weight;
            if (pos[v] < heapSize && tentative_g < gScore[v]) {
                gScore[v] = tentative_g;
                decreaseKey(v, tentative_g + heuristic(v, dest));
            }
        }
    }
}

int main() {
    graph = (Node*)calloc(NODE_COUNT_ESTIMATE, sizeof(Node));
    nodeCoords = (Coord*)malloc(NODE_COUNT_ESTIMATE * sizeof(Coord));
    gScore = (long long*)malloc(NODE_COUNT_ESTIMATE * sizeof(long long));
    minHeap = (HeapNode*)malloc(NODE_COUNT_ESTIMATE * sizeof(HeapNode));
    pos = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));

    loadData();

    printf("Iniciando Benchmark A* (Dataset Massivo)...\n");
    clock_t start = clock();
    
    a_star(ID_ORIGEM, ID_DESTINO);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    if (gScore[ID_DESTINO] == INF) printf("Caminho não encontrado!\n");
    else printf("Custo Final: %lld\n", gScore[ID_DESTINO]);
        
    printf("Tempo de Execução: %f segundos\n", time_spent);

    return 0;
}