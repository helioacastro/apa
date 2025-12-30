#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

// ============================================================
// CONFIGURAÇÃO DO BENCHMARK (AJUSTE AQUI)
// ============================================================
#define FILENAME_EDGES "brc202d_edges.csv"
#define NODE_COUNT_ESTIMATE 300000 // Tamanho estimado para alocação
#define INF 999999999

// Ajuste com os IDs que o script Python forneceu!
int ID_ORIGEM = 0;  
int ID_DESTINO = 61033; 

// ============================================================
// ESTRUTURAS DE DADOS
// ============================================================

// Lista de Adjacência
typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    Edge* head;
} Node;

Node graph[NODE_COUNT_ESTIMATE];
int num_nodes = 0;

// Min-Heap (Fila de Prioridade)
typedef struct {
    int vertex;
    int dist;
} HeapNode;

HeapNode* minHeap;
int* pos; // Mapa: Vértice -> Posição no Heap
int heapSize = 0;

// ============================================================
// FUNÇÕES DE HEAP
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

void decreaseKey(int v, int dist) {
    int i = pos[v];
    minHeap[i].dist = dist;
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
// LEITURA DE ARQUIVO
// ============================================================
void loadGraph() {
    FILE* file = fopen(FILENAME_EDGES, "r");
    if (!file) { printf("Erro ao abrir %s\n", FILENAME_EDGES); exit(1); }

    char line[1024];
    fgets(line, 1024, file); // Pula cabeçalho

    while (fgets(line, 1024, file)) {
        int u, v, w;
        // Formato: source,target,weight
        sscanf(line, "%d,%d,%d", &u, &v, &w);

        if (u >= NODE_COUNT_ESTIMATE || v >= NODE_COUNT_ESTIMATE) {
            printf("AVISO: ID de nó (%d ou %d) excede estimativa. Aumente NODE_COUNT_ESTIMATE.\n", u, v);
            continue;
        }
        if (u > num_nodes) num_nodes = u;
        if (v > num_nodes) num_nodes = v;

        // Adiciona aresta
        Edge* newEdge = (Edge*)malloc(sizeof(Edge));
        newEdge->to = v;
        newEdge->weight = w;
        newEdge->next = graph[u].head;
        graph[u].head = newEdge;
    }
    num_nodes++; // Ajuste base 0
    fclose(file);
    printf("Grafo carregado. Nós estimados: %d\n", num_nodes);
}

// ============================================================
// ALGORITMO DIJKSTRA
// ============================================================
int* dist;

void dijkstra(int src, int dest) {
    dist = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));
    minHeap = (HeapNode*)malloc(NODE_COUNT_ESTIMATE * sizeof(HeapNode));
    pos = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));

    for (int v = 0; v < num_nodes; ++v) {
        dist[v] = INF;
        minHeap[v].vertex = v;
        minHeap[v].dist = INF;
        pos[v] = v;
    }

    minHeap[src].dist = 0;
    dist[src] = 0;
    decreaseKey(src, 0);
    heapSize = num_nodes;

    while (heapSize != 0) {
        HeapNode minNode = extractMin();
        int u = minNode.vertex;

        // Otimização: Parar se chegamos ao destino
        if (u == dest) break;
        if (dist[u] == INF) break; // Inalcançável

        Edge* crawl = graph[u].head;
        while (crawl != NULL) {
            int v = crawl->to;
            if (isInMinHeap(v) && dist[u] != INF && crawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + crawl->weight;
                decreaseKey(v, dist[v]);
            }
            crawl = crawl->next;
        }
    }
}

int main() {
    // Configurar IDs manualmente se necessário ou via args
    // ID_ORIGEM = ...
    // ID_DESTINO = ...
    
    // Carrega Grafo
    for(int i=0; i<NODE_COUNT_ESTIMATE; i++) graph[i].head = NULL;
    loadGraph();

    printf("Iniciando Benchmark Dijkstra...\n");
    clock_t start = clock();
    
    // Executa várias vezes para média se for muito rápido
    int runs = 10;
    for(int i=0; i<runs; i++) {
        dijkstra(ID_ORIGEM, ID_DESTINO);
        if (i < runs-1) { // Reset simples para próxima run
             // (Em benchmark real, ideal é liberar e realocar ou resetar dists)
        }
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;

    printf("Distância Encontrada: %d\n", dist[ID_DESTINO]);
    printf("Tempo Médio de Execução: %f segundos\n", time_spent);

    return 0;
}