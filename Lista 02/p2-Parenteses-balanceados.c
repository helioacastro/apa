/* 2 Uma cadeia de caracteres contendo apenas os caracteres ‘(’ e ‘)’ é armazenada 
como um vetor de inteiros. Escreva uma função que, utilizando uma pilha, verifica se a 
cadeia de caracteres está balanceada, ou seja, para cada “abre parêntesis”, há um 
“fecha parêntesis”. A função deve retornar 1, se a cadeia é balanceada, ou 0, caso 
contrário. O protótipo da função é:     int parentesis_balanceados(int *v, int n);*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct elemento {
    char info;
    struct elemento* prox;
} Elemento;

typedef struct pilha {
    Elemento* prim;
} Pilha;

Pilha* pilha_cria(void);
void pilha_push(Pilha* p, char v);
char pilha_pop(Pilha* p);
int pilha_vazia(Pilha* p);
void pilha_libera(Pilha* p);
int parentesis_balanceados(int *v, int n);

int main(void) {
    Pilha* p = pilha_cria();

    char cadeiaCaracteres[] = "(()()())"; 
    int tamanhoCadeia = strlen(cadeiaCaracteres);
    printf("Cadeia de caracteres: %s\n", cadeiaCaracteres);

    if (parentesis_balanceados((int*)cadeiaCaracteres, tamanhoCadeia)==1) {
        printf("A cadeia de caracteres está balanceada.\n");
    } else { // Retorna 0
        printf("A cadeia de caracteres não está balanceada.\n");
    }

}   

char pilha_pop(Pilha* p) {
    Elemento* t;
    char v;
    if (pilha_vazia(p)) {
        printf("Pilha vazia\n");
        exit(1);
    }
    t = p->prim;
    v = t->info;
    p->prim = t->prox;
    free(t);
    return v;   
}
Pilha* pilha_cria(void) {
    Pilha* p = (Pilha*) malloc(sizeof(Pilha));
    p->prim = NULL;
    return p;
}
void pilha_push(Pilha* p, char v) {
    Elemento* n = (Elemento*) malloc(sizeof(Elemento));
    n->info = v;
    n->prox = p->prim;
    p->prim = n;
}
void pilha_libera(Pilha* p) {
    Elemento *t, *q = p->prim;
    while (q != NULL) {
        Elemento* t = q->prox;
        free(q);
        q = t;
    }
    free(p);
}
int pilha_vazia(Pilha* p) {
    if (p->prim == NULL){
        return 1;
    } else {
        return 0;
    }
}
int parentesis_balanceados(int *v, int n) {
    Pilha* p = pilha_cria();
    int balanceado = 0;
    // Convertendo int* para char*
    char *c = (char*) v;

    for (int i = 0; i < n; i++) {
        if (c[i] == '(') {
            pilha_push(p, v[i]);
        } else if (c[i] == ')') {
            if (pilha_vazia(p)) {
                pilha_libera(p);
                return 0; // Fecha parêntesis sem correspondente abre parêntesis
            }
            pilha_pop(p);
        }
    }
    if (pilha_vazia(p)) {
        balanceado = 1; // Todos os parêntesis foram balanceados
    } else {
        balanceado = 0; // Ainda há abre parêntesis sem correspondente fecha parêntesis
    }
    pilha_libera(p);
    return balanceado;
}