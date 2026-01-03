#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

// ============================================================
// CONFIGURAÇÃO PARA DATASET MASSIVO
// ============================================================
#define FILENAME_EDGES "massivo_edges.csv"
#define NODE_COUNT_ESTIMATE 1000005 
#define INF 2000000000LL // Long Long para evitar overflow em caminhos longos

int ID_ORIGEM = 0;  
int ID_DESTINO = 999999; // Ajustado para 1 milhão de nós (N-1)

// ============================================================
// ESTRUTURAS DE DADOS
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
long long* dist;
int num_nodes = 0;

typedef struct {
    int vertex;
    long long dist;
} HeapNode;

HeapNode* minHeap;
int* pos; 
int heapSize = 0;

// --- Funções de Heap ---
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
// CARREGAMENTO DO GRAFO
// ============================================================
void loadGraph() {
    FILE* file = fopen(FILENAME_EDGES, "r");
    if (!file) { printf("Erro ao abrir %s\n", FILENAME_EDGES); exit(1); }

    char line[1024];
    fgets(line, 1024, file); // Pula cabeçalho

    int max_id_found = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) != 3) continue;

        if (u >= NODE_COUNT_ESTIMATE || v >= NODE_COUNT_ESTIMATE) continue;
        
        if (u > max_id_found) max_id_found = u;
        if (v > max_id_found) max_id_found = v;

        Edge* newEdge = (Edge*)malloc(sizeof(Edge));
        newEdge->to = v;
        newEdge->weight = w;
        newEdge->next = graph[u].head;
        graph[u].head = newEdge;
    }
    num_nodes = max_id_found + 1;
    fclose(file);
    printf("Grafo carregado. Nós reais detectados: %d\n", num_nodes);
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

        if (u == dest) return;
        if (dist[u] == INF) break;

        for (Edge* e = graph[u].head; e; e = e->next) {
            int v = e->to;
            if (isInMinHeap(v) && dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
                decreaseKey(v, dist[v]);
            }
        }
    }
}

int main() {
    // Alocação no Heap para evitar Stack Overflow
    graph = (Node*)calloc(NODE_COUNT_ESTIMATE, sizeof(Node));
    dist = (long long*)malloc(NODE_COUNT_ESTIMATE * sizeof(long long));
    minHeap = (HeapNode*)malloc(NODE_COUNT_ESTIMATE * sizeof(HeapNode));
    pos = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));

    loadGraph();

    printf("Iniciando Benchmark Dijkstra (Dataset Massivo)...\n");
    clock_t start = clock();
    
    // Executa apenas uma vez devido ao tempo em datasets de 1M
    dijkstra(ID_ORIGEM, ID_DESTINO);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    if (dist[ID_DESTINO] == INF) printf("Caminho não encontrado!\n");
    else printf("Distancia Encontrada: %lld\n", dist[ID_DESTINO]);
        
    printf("Tempo de Execução: %f segundos\n", time_spent);

    return 0;
}