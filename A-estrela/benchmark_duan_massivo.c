#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

#define FILENAME_EDGES "massivo_edges.csv"
#define NODE_COUNT_ESTIMATE 1000001 
#define INF 2000000000
#define MAX_LEVELS 32 // Aumentado para suportar distâncias maiores
#define MAX_POOL_SIZE 8000000 // Aumentado para suportar muitos relaxamentos

int ID_ORIGEM = 0;  
int ID_DESTINO = 999999; 

typedef struct Edge {
    int to; int weight; struct Edge* next;
} Edge;

typedef struct { Edge* head; } Node;

Node* graph;
int* dists;
int num_nodes = 0;

typedef struct BucketNode {
    int vertex; struct BucketNode* next;
} BucketNode;

BucketNode* pool;
int pool_ptr = 0;

typedef struct { BucketNode* head; } Bucket;

typedef struct {
    Bucket levels[MAX_LEVELS];
    int current_min_dist;
} HierarchicalSystem;

void add_edge(int u, int v, int w) {
    Edge* e = malloc(sizeof(Edge));
    e->to = v; e->weight = w; e->next = graph[u].head;
    graph[u].head = e;
}

void load_graph() {
    FILE* file = fopen(FILENAME_EDGES, "r");
    char line[1024]; fgets(line, 1024, file);
    int max_id = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            add_edge(u, v, w);
            if (u > max_id) max_id = u; if (v > max_id) max_id = v;
        }
    }
    num_nodes = max_id + 1;
    fclose(file);
}

static inline void push_to_system(HierarchicalSystem* sys, int v, int d) {
    int diff = d - sys->current_min_dist;
    int level = 0;
    if (diff > 0) {
        level = 31 - __builtin_clz(diff);
        if (level >= MAX_LEVELS) level = MAX_LEVELS - 1;
    }
    if (pool_ptr < MAX_POOL_SIZE) {
        BucketNode* bn = &pool[pool_ptr++];
        bn->vertex = v; bn->next = sys->levels[level].head;
        sys->levels[level].head = bn;
    }
}

int extract_min_duan(HierarchicalSystem* sys) {
    for (int i = 0; i < MAX_LEVELS; i++) {
        if (sys->levels[i].head != NULL) {
            BucketNode* temp = sys->levels[i].head;
            int v = temp->vertex;
            sys->levels[i].head = temp->next;
            if (dists[v] > sys->current_min_dist) sys->current_min_dist = dists[v];
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
        if (u == -1 || u == dest) return;
        for (Edge* e = graph[u].head; e; e = e->next) {
            if (dists[u] + e->weight < dists[e->to]) {
                dists[e->to] = dists[u] + e->weight;
                push_to_system(&sys, e->to, dists[e->to]);
            }
        }
    }
}

int main() {
    graph = calloc(NODE_COUNT_ESTIMATE, sizeof(Node));
    dists = malloc(NODE_COUNT_ESTIMATE * sizeof(int));
    pool = malloc(MAX_POOL_SIZE * sizeof(BucketNode));
    load_graph();
    printf("Iniciando Benchmark Duan (Massivo)...\n");
    clock_t start = clock();
    duan_sssp(ID_ORIGEM, ID_DESTINO);
    clock_t end = clock();
    printf("Distancia: %d | Tempo: %f s\n", dists[ID_DESTINO], (double)(end - start)/CLOCKS_PER_SEC);
    return 0;
}