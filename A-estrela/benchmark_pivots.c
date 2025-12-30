#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define FILENAME_EDGES "brc202d_edges.csv"
#define NODE_COUNT_ESTIMATE 300000
#define INF 999999999

int ID_ORIGEM = 0;  
int ID_DESTINO = 61033; 

// Estruturas iguais ao Dijkstra...
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

typedef struct {
    int vertex;
    int dist;
} HeapNode;

HeapNode* minHeap;
int* pos; 
int heapSize = 0;
int* dist;

// ... (Funções de Heap do Dijkstra aqui: swap, minHeapify, extractMin, decreaseKey, isInMinHeap) ...
// (Copie as mesmas funções do benchmark_dijkstra.c para economizar espaço aqui, elas são idênticas)
// Para o código completo, vou reimplementar versões curtas:
void swap(HeapNode* a, HeapNode* b) { HeapNode t=*a; *a=*b; *b=t; }
void minHeapify(int idx) {
    int s=idx, l=2*idx+1, r=2*idx+2;
    if(l<heapSize && minHeap[l].dist < minHeap[s].dist) s=l;
    if(r<heapSize && minHeap[r].dist < minHeap[s].dist) s=r;
    if(s!=idx) { pos[minHeap[s].vertex]=idx; pos[minHeap[idx].vertex]=s; swap(&minHeap[s], &minHeap[idx]); minHeapify(s); }
}
HeapNode extractMin() {
    HeapNode r=minHeap[0], l=minHeap[heapSize-1]; minHeap[0]=l; pos[r.vertex]=heapSize-1; pos[l.vertex]=0; heapSize--; minHeapify(0); return r;
}
void decreaseKey(int v, int d) {
    int i=pos[v]; minHeap[i].dist=d;
    while(i && minHeap[i].dist < minHeap[(i-1)/2].dist) {
        pos[minHeap[i].vertex]=(i-1)/2; pos[minHeap[(i-1)/2].vertex]=i;
        swap(&minHeap[i], &minHeap[(i-1)/2]); i=(i-1)/2;
    }
}
bool isInMinHeap(int v) { return pos[v] < heapSize; }

// ============================================================
// CARREGAMENTO
// ============================================================
void loadGraph() {
    // (Mesma função loadGraph do benchmark_dijkstra.c)
    FILE* file = fopen(FILENAME_EDGES, "r");
    if (!file) exit(1);
    char line[1024]; fgets(line,1024,file);
    while (fgets(line, 1024, file)) {
        int u, v, w; sscanf(line, "%d,%d,%d", &u, &v, &w);
        if (u>num_nodes) num_nodes=u; if (v>num_nodes) num_nodes=v;
        Edge* n = malloc(sizeof(Edge)); n->to=v; n->weight=w; n->next=graph[u].head; graph[u].head=n;
    }
    num_nodes++; fclose(file);
}

// ============================================================
// FUNÇÃO FIND PIVOTS (O diferencial)
// ============================================================
// Simula a seleção de pivôs descrita na Seção 3 do artigo
int* findPivots(int src, int* dist_estimates, int W_limit, int K, int* num_selected) {
    int* pivots = (int*)malloc(num_nodes * sizeof(int));
    *num_selected = 0;
    
    // Sempre inclui a fonte
    pivots[(*num_selected)++] = src;

    // Amostragem
    for (int i = 0; i < num_nodes; i++) {
        if (i == src) continue;
        
        // Critério: Nó alcançável dentro do limite local (W_limit)
        if (dist_estimates[i] < INF && dist_estimates[i] > W_limit) {
            // Probabilidade de seleção (simulando hitting set)
            // Seleciona aproximadamente K pivôs aleatoriamente
            if ((rand() % num_nodes) < K) {
                pivots[(*num_selected)++] = i;
            }
        }
    }
    return pivots;
}

// ============================================================
// DIJKSTRA ADAPTADO (Executa fases)
// ============================================================
void dijkstra_standard(int src, int dest) {
    // (Dijkstra normal usado como sub-rotina para estimar distâncias)
    // Inicializa estruturas globais
    for(int v=0;v<num_nodes;v++){ dist[v]=INF; minHeap[v].vertex=v; minHeap[v].dist=INF; pos[v]=v; }
    minHeap[src].dist=0; dist[src]=0; decreaseKey(src,0); heapSize=num_nodes;
    while(heapSize!=0){
        HeapNode m = extractMin(); int u = m.vertex;
        if(u==dest) break; if(dist[u]==INF) break;
        for(Edge* e=graph[u].head; e; e=e->next){
            if(isInMinHeap(e->to) && dist[u]+e->weight < dist[e->to]){
                dist[e->to]=dist[u]+e->weight; decreaseKey(e->to, dist[e->to]);
            }
        }
    }
}

void new_algorithm_sssp(int src, int dest) {
    // Alocação
    dist = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));
    minHeap = (HeapNode*)malloc(NODE_COUNT_ESTIMATE * sizeof(HeapNode));
    pos = (int*)malloc(NODE_COUNT_ESTIMATE * sizeof(int));

    // Passo 1: Executa um Dijkstra Parcial/Estimativo
    // (Na prática do artigo, isso seria limitado, mas aqui rodamos completo para ter estimativas)
    dijkstra_standard(src, dest);
    
    // Passo 2: Seleciona Pivôs para Otimização
    int W_limit = 500; // Limite de peso local
    int K = (int)sqrt(num_nodes); // Raiz quadrada de N
    int num_pivots;
    
    int* pivots = findPivots(src, dist, W_limit, K, &num_pivots);
    
    // Passo 3: "Simulação" do uso dos Pivôs
    // O algoritmo real usaria esses pivôs para criar atalhos.
    // Aqui, apenas validamos que a seleção ocorreu.
    // O tempo deste algoritmo será: Tempo do Dijkstra + Tempo do FindPivots
    
    free(pivots);
    free(dist);
    free(minHeap);
    free(pos);
}

int main() {
    srand(time(NULL));
    for(int i=0; i<NODE_COUNT_ESTIMATE; i++) graph[i].head = NULL;
    loadGraph();

    printf("Iniciando Benchmark FindPivots...\n");
    clock_t start = clock();
    
    int runs = 10;
    for(int i=0; i<runs; i++) {
        new_algorithm_sssp(ID_ORIGEM, ID_DESTINO);
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;

    printf("Tempo Médio (com Overhead de Pivôs): %f segundos\n", time_spent);
    return 0;
}