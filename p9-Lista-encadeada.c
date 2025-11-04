/*9  –  Escreva  uma  função  para  trocar  os  elementos  m  e  n  de  
uma  lista  simplesmente encadeada  (m  e  n  podem  ser  chaves  ou  
mesmo  ponteiros  para  os  elementos  –  a escolha é sua). 
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
Lista* busca(Lista* lst, int v);
void lst_libera(Lista* lst);
int trocaElementos(Lista* lst, int m, int n);

int main(void){
    Lista* lst;

    lst = lst_cria();
    lst = lst_insere(lst, 10);
    lst = lst_insere(lst, 20);
    lst = lst_insere(lst, 30);
    
    lst_imprime(lst);

    trocaElementos(lst,10,30);
    printf("Após troca:\n");
    lst_imprime(lst);
    lst_libera(lst);
    return 0;
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
int lst_vazia(Lista* lst){
    return (lst == NULL);
}
Lista* busca(Lista* lst, int v){
    Lista* p;
    for(p=lst; p!=NULL; p=p->prox){
        if(p->info == v){
            return p;
        }
    }
    return NULL;
}
void lst_libera(Lista* lst){
    Lista* p = lst;
    while(p != NULL){
        Lista* t = p->prox;
        free(p);
        p = t;
    }
}
int trocaElementos(Lista* lst, int m, int n){
    Lista* pM = busca(lst, m);
    Lista* pN = busca(lst, n);
    if(pM == NULL || pN == NULL){
        return 0; // Elemento não encontrado
    }
    int temp = pM->info;
    pM->info = pN->info;
    pN->info = temp;
    return 1; // Troca realizada com sucesso
}