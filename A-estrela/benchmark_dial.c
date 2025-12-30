#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

#define FILENAME_EDGES "brc202d_edges.csv"
#define NODE_COUNT_ESTIMATE 300000 
#define INF 999999999

// Ajuste com os IDs reais do seu mapa
int ID_ORIGEM = 0;  
int ID_DESTINO = 61033; 

// Estrutura de Grafo
typedef struct Edge {
    int to;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    Edge* head;
} Node;

Node* graph;
int num_nodes = 0;
int max_weight = 0; // Necessário para dimensionar os buckets

// Estrutura de Buckets (Dial's Implementation)
// Cada bucket é uma lista duplamente encadeada de nós
typedef struct BucketNode {
    int vertex;
    struct BucketNode *prev, *next;
} BucketNode;

// Array de cabeças de buckets. Tamanho = MaxDist (Cuidado com memória!)
// Como pesos são 10/14, a distância máxima não explode tanto, mas pode ser grande.
// Usaremos um array circular de buckets de tamanho (MaxEdgeWeight + 1).
// Isso é uma otimização do Dial para economizar memória.
typedef struct {
    BucketNode* head;
} Bucket;

Bucket* buckets;
int bucket_size; // Tamanho do array circular (MaxWeight + 1)
int current_bucket_idx = 0; // Cursor atual

int* dist;
// Precisamos guardar o ponteiro do nó na lista para remoção rápida (update)
BucketNode** node_pointers; 

// ============================================================
// FUNÇÕES DE BUCKET
// ============================================================
void add_to_bucket(int v, int d) {
    int idx = d % bucket_size;
    BucketNode* bn = (BucketNode*)malloc(sizeof(BucketNode));
    bn->vertex = v;
    bn->prev = NULL;
    bn->next = buckets[idx].head;
    if (buckets[idx].head) buckets[idx].head->prev = bn;
    buckets[idx].head = bn;
    node_pointers[v] = bn;
}

void remove_from_bucket(int v, int d) {
    if (d == INF) return;
    int idx = d % bucket_size;
    BucketNode* bn = node_pointers[v];
    if (!bn) return;
    
    if (bn->prev) bn->prev->next = bn->next;
    else buckets[idx].head = bn->next;
    
    if (bn->next) bn->next->prev = bn->prev;
    
    free(bn);
    node_pointers[v] = NULL;
}

// ============================================================
// CARREGAMENTO
// ============================================================
void loadGraph() {
    FILE* file = fopen(FILENAME_EDGES, "r");
    if (!file) exit(1);
    char line[1024]; fgets(line, 1024, file);
    int max_id = 0;
    while (fgets(line, 1024, file)) {
        int u, v, w;
        if (sscanf(line, "%d,%d,%d", &u, &v, &w) == 3) {
            if (u>max_id) max_id=u; if(v>max_id) max_id=v;
            if (w > max_weight) max_weight = w;
            Edge* n = malloc(sizeof(Edge)); n->to=v; n->weight=w; n->next=graph[u].head; graph[u].head=n;
        }
    }
    num_nodes = max_id + 1;
    fclose(file);
    printf("Grafo carregado. Max Weight: %d\n", max_weight);
}

// ============================================================
// DIJKSTRA COM DIAL (BUCKETS)
// ============================================================
void dijkstra_dial(int src, int dest) {
    // Reset
    for (int v = 0; v < num_nodes; ++v) {
        dist[v] = INF;
        node_pointers[v] = NULL;
    }
    
    // Limpa buckets (apenas os que vamos usar)
    // Otimização: Não precisamos limpar tudo se usarmos current_idx corretamente,
    // mas por segurança limpamos.
    for(int i=0; i<bucket_size; i++) buckets[i].head = NULL;

    dist[src] = 0;
    add_to_bucket(src, 0);
    current_bucket_idx = 0;
    
    int nodes_processed = 0;
    int max_possible_dist = num_nodes * max_weight; // Segurança para o loop

    // Loop principal: Avança o cursor do bucket até achar um não vazio
    // Como é circular, usamos a distância absoluta para controlar o loop
    int current_dist = 0;
    
    while (nodes_processed < num_nodes && current_dist < max_possible_dist) {
        int idx = current_dist % bucket_size;
        
        while (buckets[idx].head == NULL) {
            current_dist++;
            if (current_dist > max_possible_dist) break; // Evita loop infinito se desconexo
            idx = current_dist % bucket_size;
        }
        if (current_dist > max_possible_dist) break;

        // Extrai todos os nós deste bucket (distância mínima igual)
        BucketNode* bn = buckets[idx].head;
        while (bn != NULL) {
            int u = bn->vertex;
            BucketNode* next_bn = bn->next;
            
            // Remove do bucket (já temos o ponteiro, manual é mais rápido que remove_from_bucket)
            // Mas para simplicidade, apenas marcamos processado e seguimos
            // Na implementação real do Dial, removemos da lista.
            buckets[idx].head = next_bn; 
            if(next_bn) next_bn->prev = NULL;
            free(bn); // Libera nó do bucket
            node_pointers[u] = NULL;
            
            nodes_processed++;
            if (u == dest) return;

            // Relaxamento
            for (Edge* e = graph[u].head; e; e = e->next) {
                int v = e->to;
                int w = e->weight;
                if (dist[u] + w < dist[v]) {
                    if (dist[v] != INF) {
                        remove_from_bucket(v, dist[v]);
                    }
                    dist[v] = dist[u] + w;
                    add_to_bucket(v, dist[v]);
                }
            }
            
            bn = buckets[idx].head; // Recarrega head caso tenha mudado
        }
    }
}

int main() {
    graph = malloc(NODE_COUNT_ESTIMATE * sizeof(Node));
    dist = malloc(NODE_COUNT_ESTIMATE * sizeof(int));
    node_pointers = malloc(NODE_COUNT_ESTIMATE * sizeof(BucketNode*));
    
    for(int i=0; i<NODE_COUNT_ESTIMATE; i++) graph[i].head = NULL;
    loadGraph();

    // Configura Buckets
    // Dial otimizado: Tamanho = Maior peso de aresta + 1
    // Isso funciona porque a nova distância nunca será maior que (atual + max_weight)
    bucket_size = max_weight + 1;
    buckets = malloc(bucket_size * sizeof(Bucket));

    ID_ORIGEM = 0;
    ID_DESTINO = num_nodes - 1;

    printf("Iniciando Benchmark Dial (Proxy para Buckets)...\n");
    clock_t start = clock();
    
    int runs = 10;
    for(int i=0; i<runs; i++) {
        dijkstra_dial(ID_ORIGEM, ID_DESTINO);
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC / runs;

    if (dist[ID_DESTINO] == INF) printf("Caminho não encontrado!\n");
    else printf("Distância: %d\n", dist[ID_DESTINO]);
    printf("Tempo Médio: %f segundos\n", time_spent);

    return 0;
}