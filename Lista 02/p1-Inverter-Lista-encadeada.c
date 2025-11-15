/*
1 – Escreva um procedimento não recursivo, com tempo de execução Θ(n) que inverta 
uma lista simplesmente encadeada de n elementos. Além do custo de armazenar os n 
elementos, o procedimento não deve gastar mais do que O(1) para inverter a lista
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct lista {
    int info;
    struct lista* prox;
} Lista;

Lista* lst_cria();
Lista* lst_insere(Lista* lst, int val);
void lst_imprime(Lista* lst);
int lst_vazia(Lista* lst);
Lista* lst_inverte(Lista* lst);

int main(void){
    Lista* lst;

    lst = lst_cria();
    lst = lst_insere(lst, 10);
    lst = lst_insere(lst, 20);
    lst = lst_insere(lst, 30);
    
    printf("Lista original:\n");
    lst_imprime(lst);

    lst = lst_inverte(lst); 
    printf("Lista invertida:\n");
    lst_imprime(lst);
}

Lista* lst_cria() {
    return NULL;
}

Lista* lst_insere(Lista* lst, int val){
    Lista* novo = (Lista*) malloc(sizeof(Lista));
    novo->info = val;
    novo->prox = lst;
    return novo;
}

void lst_imprime(Lista* lst){
    Lista* p;
    for(p=lst; p!=NULL; p=p->prox){
        printf("info = %d\n", p->info);
    }
}
Lista* lst_inverte(Lista* lst){
    Lista* anterior = NULL;
    Lista* atual = lst;
    Lista* proxima = NULL;

    while (atual != NULL) {
        proxima = atual->prox;  // Armazena o próximo nó
        atual->prox = anterior;  // Inverte o ponteiro do nó atual
        anterior = atual;        // Move anterior para o nó atual
        atual = proxima;       // Avança para o próximo nó
    }
    return anterior; // Novo cabeçalho da lista invertida
}