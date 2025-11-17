/*2 - Escreva um programa que implementa funções para: a) contar o número de nós em uma árvore 
binária; b) contar o número de folhas; c) contar o número de filhos à direita; d) contar a altura da 
árvore. */

#include <stdio.h>
#include <stdlib.h> 

typedef struct arv {
    int info;
    struct arv* sae;
    struct arv* sad;
} Arv;

// Protótipos das funções
Arv* arv_criavazia(void);
Arv* arv_cria(char c, Arv* sae, Arv* sad);
Arv* arv_libera(Arv* a);
int arv_vazia(Arv* a);
int arv_pertence(Arv* a, char c);
void arv_imprime(Arv* a);

int arv_numero_nos(Arv* a);
int arv_numero_folhas(Arv* a);
int arv_numero_filhos_direita(Arv* a);
int arv_altura(Arv* a);

int main(void){
    // Criando uma árvore binária de exemplo
    //         A
    //        / \
    //       B   C
    //      / \   \
    //     D   E   F
    Arv* d = arv_cria('D', arv_criavazia(), arv_criavazia());
    Arv* e = arv_cria('E', arv_criavazia(), arv_criavazia());
    Arv* f = arv_cria('F', arv_criavazia(), arv_criavazia());
    Arv* b = arv_cria('B', d, e);
    Arv* c = arv_cria('C', arv_criavazia(), f);
    Arv* a = arv_cria('A', b, c); 

    printf("Estrutura da árvore: "); 
    arv_imprime(a); 
    printf("\n");

    printf("Número de nós: %d\n", arv_numero_nos(a));
    printf("Número de folhas: %d\n", arv_numero_folhas(a));
    printf("Número de filhos à direita: %d\n", arv_numero_filhos_direita(a));
    printf("Altura da árvore: %d\n", arv_altura(a));

    // Liberando a memória da árvore
    arv_libera(a);

    return 0;
}

Arv* arv_criavazia(void){
    return NULL;
}
Arv* arv_cria(char c, Arv* sae, Arv* sad){
    Arv* p = (Arv*) malloc(sizeof(Arv));
    p->info = c;
    p->sae = sae;
    p->sad = sad;
    return p;
}
Arv* arv_libera(Arv* a){
    if(!arv_vazia(a)){
        arv_libera(a->sae);
        arv_libera(a->sad);
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
        arv_pertence(a->sae, c) || 
        arv_pertence(a->sad, c);
    }
}
void arv_imprime(Arv* a){
    printf("<");
    if(!arv_vazia(a)){
        printf("%c ", a->info);
        arv_imprime(a->sae);
        arv_imprime(a->sad);
    }
    printf(">");
}
int arv_numero_nos(Arv* a) {
    if (arv_vazia(a)) {
        return 0;
    } else {
        return 1 + arv_numero_nos(a->sae) + arv_numero_nos(a->sad);
    }
}
int arv_altura(Arv* a) {
    if (arv_vazia(a)) {
        return -1; // Altura de árvore vazia é -1
    } else {
        int he = arv_altura(a->sae);
        int hd = arv_altura(a->sad);
        return (he > hd ? he : hd) + 1;
    }
}
int arv_numero_folhas(Arv* a) {
    if (arv_vazia(a)) {
        return 0;
    } 
    // Nó folha: não tem filhos
    if (arv_vazia(a->sae) && arv_vazia(a->sad)) {
        return 1;
    } 
    // Soma folhas dos subárvores
    return arv_numero_folhas(a->sae) + arv_numero_folhas(a->sad);
}
int arv_numero_filhos_direita(Arv* a) {
    if (arv_vazia(a)) {
        return 0;
    } 
    int count = 0;
    // Verifica se o filho direito existe
    if (!arv_vazia(a->sad)) {
        count = 1; // Conta este filho direito
    }
    // Soma filhos direitos dos subárvores
    return count + arv_numero_filhos_direita(a->sae) + arv_numero_filhos_direita(a->sad);
}