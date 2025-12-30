#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define FILENAME_EDGES "brc202d_edges.csv"
#define FILENAME_NODES "brc202d_nodes.csv"
#define NODE_COUNT_ESTIMATE 300000 
#define INF 999999999

int ID_ORIGEM = 0;  
int ID_DESTINO = 61033; 

// Estrutura para Coordenadas
typedef struct {
    double x, y;
} Coord;

Coord nodeCoords[NODE_COUNT_ESTIMATE];

// Grafo
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

// Min-Heap (Armazena fScore = g + h)
typedef struct {
    int vertex;
    double fScore; // Float pois a heurística é Euclidiana
} HeapNode;

HeapNode* minHeap;
int* pos; 
int heapSize = 0;

// ============================================================
// HEURÍSTICA OCTILE (OTIMIZADA PARA MAPAS 8-VIAS)
// ============================================================
// Custo Reto = 10, Diagonal = 14
int heuristic(int u, int target) {
    int dx = abs(nodeCoords[u].x - nodeCoords[target].x);
    int dy = abs(nodeCoords[u].y - nodeCoords[target].y);
    
    // Lógica: Andar na diagonal o máximo possível (custo 14), 
    // e o restante em linha reta (custo 10).
    if (dx > dy)
        return 14 * dy + 10 * (dx - dy);
    else
        return 14 * dx + 10 * (dy - dx);
}


// ... (Funções de Heap similares ao Dijkstra, mas usando fScore double) ...
void swap(HeapNode* a, HeapNode* b) { HeapNode t = *a; *a = *b; *b = t; }

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

void decreaseKey(int v, double fScore) {
    int i = pos[v];
    minHeap[i].fScore = fScore;
    while (i && minHeap[i].fScore < minHeap[(i - 1) / 2].fScore) {
        pos[minHeap[i].vertex] = (i - 1) / 2;
        pos[minHeap[(i - 1) / 2].vertex] = i;
        swap(&minHeap[i], &minHeap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

bool isInMinHeap(int v) { return pos[v] < heapSize; }

// ============================================================
// CARREGAMENTO
// ============================================================
void loadData() {
    // 1. Carregar Nós (Coords)
    FILE* file = fopen(FILENAME_NODES, "r");
    if (!file) { printf("Erro nós\n"); exit(1); }
    char line[1024];
    fgets(line, 1024, file); 
    while (fgets(line, 1024, file)) {
        int id;
        long long osmid;
        double x, y;
        sscanf(line, "%d,%lld,%lf,%lf", &id, &osmid, &x, &y);
        if (id < NODE_COUNT_ESTIMATE) {
            nodeCoords[id].x = x;
            nodeCoords[id].y = y;
        }
    }
    fclose(file);

    // 2. Carregar Arestas
    file = fopen(FILENAME_EDGES, "r");
    if (!file) { printf("Erro arestas\n"); exit(1); }
    fgets(line, 1024, file);
    while (fgets(line, 1024, file)) {
        int u, v, w;
        sscanf(line, "%d,%d,%d", &u, &v, &w);
        if (u > num_nodes) num_nodes = u;
        if (v > num_nodes) num_nodes = v;
        Edge* newEdge = (Edge*)malloc(sizeof(Edge));
        newEdge->to = v;
        newEdge->weight = w;
        newEdge->next = graph[u].head;
        graph[u].head = newEdge;
    }
    num_nodes++;
    fclose(file);
}

// ============================================================
// ALGORITMO A*
// ============================================================
double* gScore; // Custo real da origem até aqui

void a_star(int src, int dest) {
    gScore = (double*)malloc(NODE_COUNT_ESTIMATE * sizeof(double));
    minHeap = (HeapNode*)malloc(NODE_COUNT_ESTIMATE * sizeof(HeapNode));
    pos = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));

    for (int v = 0; v < num_nodes; ++v) {
        gScore[v] = INF;
        minHeap[v].vertex = v;
        minHeap[v].fScore = INF;
        pos[v] = v;
    }

    gScore[src] = 0;
    minHeap[src].fScore = heuristic(src, dest);
    
    // Gambiarra de Heap: coloca src no topo
    pos[src] = 0; pos[0] = src;
    swap(&minHeap[src], &minHeap[0]);
    heapSize = num_nodes;
    minHeapify(0); // Garante ordem

    while (heapSize != 0) {
        HeapNode minNode = extractMin();
        int u = minNode.vertex;

        if (u == dest) break;
        if (gScore[u] == INF) break;

        Edge* crawl = graph[u].head;
        while (crawl != NULL) {
            int v = crawl->to;
            double tentative_g = gScore[u] + crawl->weight;

            if (isInMinHeap(v) && tentative_g < gScore[v]) {
                gScore[v] = tentative_g;
                double f = tentative_g + heuristic(v, dest);
                decreaseKey(v, f);
            }
            crawl = crawl->next;
        }
    }
}

int main() {
    for(int i=0; i<NODE_COUNT_ESTIMATE; i++) graph[i].head = NULL;
    loadData();

    printf("Iniciando Benchmark A*...\n");
    clock_t start = clock();
    
    int runs = 10;
    for(int i=0; i<runs; i++) a_star(ID_ORIGEM, ID_DESTINO);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;

    printf("Custo Encontrado (gScore): %.2f\n", gScore[ID_DESTINO]);
    printf("Tempo Médio: %f segundos\n", time_spent);
    return 0;
}