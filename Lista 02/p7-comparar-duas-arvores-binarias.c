/*7 - Duas árvores binárias são similares se elas são vazias ou se elas não são vazias e 
suas subárvores da esquerda são similares e suas subárvores da direita são também 
similares. Escreva um programa para determinar se duas árvores binárias são similares*/

#include <stdio.h>
#include <stdlib.h>

// ----------------------------------------------------------------------
// ESTRUTURAS
// ----------------------------------------------------------------------

typedef struct arv {
    char info;
    struct arv* esq;
    struct arv* dir;
} Arv;

// ----------------------------------------------------------------------
// PROTÓTIPOS
// ----------------------------------------------------------------------

// Funções Base da Árvore
Arv* arv_criavazia(void);
Arv* arv_cria(char c, Arv* sae, Arv* sad);
Arv* arv_libera(Arv* a);
int arv_vazia(Arv* a);
void arv_imprime(Arv* a); // (Impressão em pré-ordem)

// Função da Questão 7
int arv_similares(Arv* a, Arv* b);


// ----------------------------------------------------------------------
// FUNÇÃO MAIN DE TESTE
// ----------------------------------------------------------------------
int main(void){
    
    // --- Testes da Questão 7 (Similaridade) ---
    
    // Árvore 1 (Exemplo)
    //    A
    //   / \
    //  B   C
    // /
    //D
    Arv* t1_d = arv_cria('D', arv_criavazia(), arv_criavazia());
    Arv* t1_b = arv_cria('B', t1_d, arv_criavazia());
    Arv* t1_c = arv_cria('C', arv_criavazia(), arv_criavazia());
    Arv* arvore1 = arv_cria('A', t1_b, t1_c);

    // Árvore 2 (Similar à Árvore 1)
    //    X
    //   / \
    //  Y   Z
    // /
    //W
    Arv* t2_w = arv_cria('W', arv_criavazia(), arv_criavazia());
    Arv* t2_y = arv_cria('Y', t2_w, arv_criavazia());
    Arv* t2_z = arv_cria('Z', arv_criavazia(), arv_criavazia());
    Arv* arvore2 = arv_cria('X', t2_y, t2_z);

    // Árvore 3 (NÃO Similar à Árvore 1)
    //    X
    //   / \
    //  Y   Z
    //   \  <-- (Diferença estrutural aqui)
    //    W
    Arv* t3_w = arv_cria('W', arv_criavazia(), arv_criavazia());
    Arv* t3_y = arv_cria('Y', arv_criavazia(), t3_w); // Filho na direita
    Arv* t3_z = arv_cria('Z', arv_criavazia(), arv_criavazia());
    Arv* arvore3 = arv_cria('X', t3_y, t3_z);
    
    // Árvore 4 (NÃO Similar à Árvore 1)
    //    X
    //   / \
    //  Y   Z
    //     / <-- (Diferença estrutural aqui)
    //    W
    Arv* t4_w = arv_cria('W', arv_criavazia(), arv_criavazia());
    Arv* t4_y = arv_cria('Y', arv_criavazia(), arv_criavazia());
    Arv* t4_z = arv_cria('Z', t4_w, arv_criavazia()); // Filho na esquerda
    Arv* arvore4 = arv_cria('X', t4_y, t4_z);


    printf("--- Teste 7: Similaridade Estrutural ---\n");
    printf("Arvore 1: "); arv_imprime(arvore1); printf("\n");
    printf("Arvore 2: "); arv_imprime(arvore2); printf("\n");
    printf("Arvore 3: "); arv_imprime(arvore3); printf("\n");
    printf("Arvore 4: "); arv_imprime(arvore4); printf("\n");
    
    printf("\n(Q7) Arvore 1 e 2 sao similares? %s\n", 
           arv_similares(arvore1, arvore2) ? "SIM" : "NAO"); // Esperado: SIM
           
    printf("(Q7) Arvore 1 e 3 sao similares? %s\n", 
           arv_similares(arvore1, arvore3) ? "SIM" : "NAO"); // Esperado: NAO
           
    printf("(Q7) Arvore 1 e 4 sao similares? %s\n", 
           arv_similares(arvore1, arvore4) ? "SIM" : "NAO"); // Esperado: NAO

    printf("(Q7) Arvore 1 e 1 sao similares? %s\n", 
           arv_similares(arvore1, arvore1) ? "SIM" : "NAO"); // Esperado: SIM
           
    printf("--------------------------------------------\n");

    arv_libera(arvore1);
    arv_libera(arvore2);
    arv_libera(arvore3);
    arv_libera(arvore4);
    
    return 0;
}


// ----------------------------------------------------------------------
// IMPLEMENTAÇÃO DA QUESTÃO 7
// ----------------------------------------------------------------------

/**
 * (7) Checa se duas árvores binárias são SIMILARES (mesma estrutura).
 * Retorna 1 (true) ou 0 (false).
 */
int arv_similares(Arv* a, Arv* b) {
    
    // PASSO 1: Caso Base (Ambas Vazias)
    // Se ambas as árvores (ou subárvores) são nulas (vazias),
    // elas são estruturalmente idênticas (similares).
    if (arv_vazia(a) && arv_vazia(b)) {
        return 1; // 1 = true (SIM)
    }

    // PASSO 2: Caso Base (Apenas UMA é Vazia)
    // Se chegamos aqui, sabemos que NÃO são ambas vazias (pelo Passo 1).
    // Se uma delas for vazia e a outra não, elas têm estruturas diferentes.
    if (arv_vazia(a) || arv_vazia(b)) {
        return 0; // 0 = false (NAO)
    }

    // PASSO 3: Caso Recursivo (Ambas NÃO são Vazias)
    // Se ambas existem (não são nulas), elas só são similares se:
    // 1. A subárvore esquerda de 'a' for similar à subárvore esquerda de 'b'
    // E (&&)
    // 2. A subárvore direita de 'a' for similar à subárvore direita de 'b'
    //
    // Se AMBAS as chamadas recursivas retornarem 1 (true),
    // o resultado de (1 && 1) será 1 (true).
    // Se QUALQUER UMA delas retornar 0 (false),
    // o resultado será 0 (false).
    return arv_similares(a->esq, b->esq) && 
           arv_similares(a->dir, b->dir);
}


// ----------------------------------------------------------------------
// IMPLEMENTAÇÕES DE BASE (Arv)
// ----------------------------------------------------------------------

Arv* arv_criavazia(void){
    return NULL;
}

Arv* arv_cria(char c, Arv* sae, Arv* sad){
    Arv* p = (Arv*) malloc(sizeof(Arv));
    if (p == NULL) exit(1);
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
    // Retorna 1 (true) se a == NULL, ou 0 (false) caso contrário
    return a == NULL;
}

// Impressão em pré-ordem para visualização
void arv_imprime(Arv* a){
    printf("<");
    if(!arv_vazia(a)){
        printf("%c", a->info);
        arv_imprime(a->esq);
        arv_imprime(a->dir);
    }
    printf(">");
}
