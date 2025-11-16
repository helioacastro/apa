/*6 – Implemente um algoritmo que determine se uma árvore binária é: (a) estritamente 
binária; (b) completa; (c) quase completa*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

typedef struct arv {
    char info;
    struct arv* esq;
    struct arv* dir;
} Arv;


Arv* arv_criavazia(void);
Arv* arv_cria(char c, Arv* sae, Arv* sad);
Arv* arv_libera(Arv* a);
int arv_vazia(Arv* a);
int arv_pertence(Arv* a, char c);
void arv_imprime(Arv* a);

int arv_altura(Arv* a);
int arv_numero_nos(Arv* a);

int arv_estritamente_binaria(Arv* a);
int arv_completa(Arv* a);
int arv_quase_completa(Arv* a);
int arv_quase_completa_rec(Arv* a, int i, int n);


int main(void){
     // --- Teste 1: Arvore COMPLETA (Perfeita) ---
    // Propriedades: (Estrita: SIM), (Completa: SIM), (Quase Completa: SIM)
    //        A
    //       / \
    //      B   C
    //     / \ / \
    //    D  E F  G
    printf("--- Teste 1: Arvore Completa (Perfeita) ---\n");
    Arv* d1 = arv_cria('D', arv_criavazia(), arv_criavazia());
    Arv* e1 = arv_cria('E', arv_criavazia(), arv_criavazia());
    Arv* f1 = arv_cria('F', arv_criavazia(), arv_criavazia());
    Arv* g1 = arv_cria('G', arv_criavazia(), arv_criavazia());
    Arv* b1 = arv_cria('B', d1, e1);
    Arv* c1 = arv_cria('C', f1, g1);
    Arv* a_completa = arv_cria('A', b1, c1); 

    printf("Estrutura: "); arv_imprime(a_completa); printf("\n");


    printf("(a) Estritamente Binaria? %s\n", arv_estritamente_binaria(a_completa) ? "SIM" : "NAO");
    printf("(b) Completa (Perfeita)?  %s\n", arv_completa(a_completa) ? "SIM" : "NAO");
    printf("(c) Quase Completa?       %s\n", arv_quase_completa(a_completa) ? "SIM" : "NAO");
    printf("--------------------------------------------\n");
    arv_libera(a_completa);


    // --- Teste 2: Arvore QUASE COMPLETA ---
    // Propriedades: (Estrita: NAO), (Completa: NAO), (Quase Completa: SIM)
    //        A
    //       / \
    //      B   C
    //     / \ / 
    //    D  E F 
    printf("--- Teste 2: Arvore Quase Completa ---\n");
    Arv* d2 = arv_cria('D', arv_criavazia(), arv_criavazia());
    Arv* e2 = arv_cria('E', arv_criavazia(), arv_criavazia());
    Arv* b2 = arv_cria('B', d2, e2);
    Arv* f2 = arv_cria('F', arv_criavazia(), arv_criavazia());
    Arv* c2 = arv_cria('C', f2, arv_criavazia()); // C só tem 1 filho (F2)
    Arv* a_quase = arv_cria('A', b2, c2); 

    printf("Estrutura: "); arv_imprime(a_quase); printf("\n");
    printf("(a) Estritamente Binaria? %s\n", arv_estritamente_binaria(a_quase) ? "SIM" : "NAO");
    printf("(b) Completa (Perfeita)?  %s\n", arv_completa(a_quase) ? "SIM" : "NAO");
    printf("(c) Quase Completa?       %s\n", arv_quase_completa(a_quase) ? "SIM" : "NAO");
    printf("--------------------------------------------\n");
    arv_libera(a_quase);


    // --- Teste 3: Arvore ESTRITAMENTE BINÁRIA (mas não Completa) ---
    // Propriedades: (Estrita: SIM), (Completa: NAO), (Quase Completa: NAO)
    //        A
    //       / \
    //      B   C
    //         / \
    //        F   G
    printf("--- Teste 3: Arvore Estritamente Binaria (Nao Completa) ---\n");
    Arv* f3 = arv_cria('F', arv_criavazia(), arv_criavazia());
    Arv* g3 = arv_cria('G', arv_criavazia(), arv_criavazia());
    Arv* b3 = arv_cria('B', arv_criavazia(), arv_criavazia()); // B é folha
    Arv* c3 = arv_cria('C', f3, g3);
    Arv* a_estrita = arv_cria('A', b3, c3); 

    printf("Estrutura: "); arv_imprime(a_estrita); printf("\n");
    printf("(a) Estritamente Binaria? %s\n", arv_estritamente_binaria(a_estrita) ? "SIM" : "NAO");
    printf("(b) Completa (Perfeita)?  %s\n", arv_completa(a_estrita) ? "SIM" : "NAO");
    printf("(c) Quase Completa?       %s\n", arv_quase_completa(a_estrita) ? "SIM" : "NAO");
    printf("--------------------------------------------\n");
    arv_libera(a_estrita);

    // --- Teste 4: Arvore Inválida (NENHUMA) ---
    // Propriedades: (Estrita: NAO), (Completa: NAO), (Quase Completa: NAO)
    //        A
    //       / \
    //      B   C
    //       \ 
    //        E
    printf("--- Teste 4: Arvore Invalida ('Buraco' na esquerda) ---\n");
    Arv* e4 = arv_cria('E', arv_criavazia(), arv_criavazia());
    Arv* b4 = arv_cria('B', arv_criavazia(), e4); // Nó B tem só filho DIR
    Arv* c4 = arv_cria('C', arv_criavazia(), arv_criavazia());
    Arv* a_invalida = arv_cria('A', b4, c4); 

    printf("Estrutura: "); arv_imprime(a_invalida); printf("\n");
    printf("(a) Estritamente Binaria? %s\n", arv_estritamente_binaria(a_invalida) ? "SIM" : "NAO");
    printf("(b) Completa (Perfeita)?  %s\n", arv_completa(a_invalida) ? "SIM" : "NAO");
    printf("(c) Quase Completa?       %s\n", arv_quase_completa(a_invalida) ? "SIM" : "NAO");
    printf("--------------------------------------------\n");
    arv_libera(a_invalida);
    
    return 0;
}

Arv* arv_criavazia(void){
    return NULL;
}
Arv* arv_cria(char c, Arv* sae, Arv* sad){
    Arv* p = (Arv*) malloc(sizeof(Arv));
    p->info = c;
    p->esq = sae;
    p->dir = sad;
    return p;
}
Arv* arv_libera(Arv* a){
    if(!arv_vazia(a)){
        arv_libera(a->esq);
        arv_libera(a->dir);
        free(a);
    }
    return NULL;
}
int arv_vazia(Arv* a){
    return (a == NULL);
}
int arv_pertence(Arv* a, char c){
    if(arv_vazia(a)){
        return 0;
    }else{
        return (a->info == c) || 
        arv_pertence(a->esq, c) || 
        arv_pertence(a->dir, c);
    }
}
void arv_imprime(Arv* a){
    printf("<");
    if(!arv_vazia(a)){
        printf("%c ", a->info);
        arv_imprime(a->esq);
        arv_imprime(a->dir);
    }
    printf(">");
}

int arv_estritamente_binaria(Arv* a) {
    // 1. Caso Base: Árvore vazia
    // Uma árvore vazia (NULL) não viola a regra, então é '1' (true).
    if (arv_vazia(a)) {
        return 1;
    }

    // 2. Caso Base: Nó Folha
    // Se ambos os filhos são nulos (0 filhos), este nó está OK.
    if (arv_vazia(a->esq) && arv_vazia(a->dir)) {
        return 1;
    }

    // 3. Caso Recursivo: Nó Interno (2 Filhos)
    // Mas precisamos verificar se os filhos DELE também estão OK.
    if (!arv_vazia(a->esq) && !arv_vazia(a->dir)) {
        // Retorna 1 (true) APENAS se a sub-árvore esquerda E a sub-árvore direita
        // também retornarem 1 (true). O '&&' funciona com 1 e 0.
        return arv_estritamente_binaria(a->esq) && arv_estritamente_binaria(a->dir);
    }
    
    // 4. Caso de Falha: (1 Filho)
// ... (existing code) ...
    // - Não tem 2 filhos (Passo 3 falhou)
    // Conclusão: O nó tem exatamente 1 filho, o que viola a regra.
    return 0;
}

int arv_completa(Arv* a) {
    // Caso especial: árvore vazia (h=-1, n=0).
    // pow(2, -1 + 1) - 1 = pow(2, 0) - 1 = 1 - 1 = 0.
    // A fórmula funciona para árvores vazias.
    
    int h = arv_altura(a); // Helper que calcula a altura
    int n = arv_numero_nos(a);    // Helper que conta os nós
    
    // Calcula o número de nós esperado para uma árvore perfeita de altura h
    // (pow(2, h + 1) - 1)
    // Nota: pow() retorna double, precisamos converter para int/long.
    long n_esperado = (long)pow(2, h + 1) - 1;
    
    // Compara o número real de nós (n) com o esperado (n_esperado)
    if (n == n_esperado) {
        //printf("A árvore é completa.\n");
        return 1;
    } else {
        //printf("A árvore NÃO é completa.\n");
        return 0;
    }
}
int arv_quase_completa_rec(Arv* a, int i, int n) {
    // Caso Base 1: Chegamos a um nó NULL (ramo vazio).
    // Se um ramo termina (NULL), ele é válido (1).
    if (arv_vazia(a)) {
        return 1;
    }

    // Caso Base 2 (Falha): O nó existe, mas seu índice 'i'
    // é maior ou igual ao total de nós 'n'.
    // Isso significa que encontramos um nó "real" (não-NULL)
    // onde deveria haver um "buraco" (NULL).
    if (i >= n) {
        return 0; // FALHA (Viola a estrutura de Heap)
    }

    // Caso Recursivo: O nó atual (i) é válido (i < n).
    // Devemos checar se seus filhos também são válidos nos
    // índices corretos do "heap/array".
    // Filho esquerdo: índice 2*i + 1
    // Filho direito: índice 2*i + 2
    // Ambos (&&) devem retornar 1 (true) para a árvore ser válida.
    return arv_quase_completa_rec(a->esq, 2 * i + 1, n) &&
           arv_quase_completa_rec(a->dir, 2 * i + 2, n);
}

int arv_quase_completa(Arv* a) {
       // 1. Contamos o número total de nós (N) na árvore.
    int n = arv_numero_nos(a); 
    
    // 2. Iniciamos a checagem recursiva.
    // A raiz (a) começa no índice i = 0.
    return arv_quase_completa_rec(a, 0, n);
}
int arv_altura(Arv* a) {
    if (arv_vazia(a)) {
        return -1; // Altura de árvore vazia é -1
    } else {
        int he = arv_altura(a->esq);
        int hd = arv_altura(a->dir);
        return (he > hd ? he : hd) + 1;
    }
}
int arv_numero_nos(Arv* a) {
    if (arv_vazia(a)) {
        return 0;
    } else {
        return 1 + arv_numero_nos(a->esq) + arv_numero_nos(a->dir);
    }
}