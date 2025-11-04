/*10 – Escreva uma função void MoveMenor(TipoLista Lista) que, dada uma lista 
com um número qualquer de elementos, acha o menor elemento da lista e o move 
para o começo da lista, como exemplificado na figura abaixo. 
(Obs. Não vale trocar apenas os campos item  ou  usar  uma  lista/fila/pilha  
auxiliar!  Você  deverá  fazer  a  manipulação  dos apontadores para 
trocar as células de posição). 
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
void MoveMenor(Lista* lst);

int main(void){
    Lista* lst;
    lst = lst_cria();
    lst = lst_insere(lst, 10);
    lst = lst_insere(lst, 21);
    lst = lst_insere(lst, 4);
    lst = lst_insere(lst, 13);
    lst_imprime(lst);



    lst_libera(lst);
    return 0;
}
Lista* lst_cria(){
    return NULL;
}
Lista* lst_insere(Lista* lst, int val){
    Lista* novo = (Lista*) malloc (sizeof(Lista));
    novo->info = val;
    novo->prox = lst;
    return novo;
}
void lst_imprime(Lista* lst){
    Lista* p;
    for(p = lst; p != NULL; p = p->prox){
        printf("info = %d \n", p->info);
    }
    printf("\n");
}
int lst_vazia(Lista* lst){
    return (lst == NULL);
}
Lista* busca(Lista* lst, int v){
    Lista* p;
    for(p = lst; p != NULL; p = p->prox){
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
void MoveMenor(Lista* lst){
    if(lst == NULL || lst->prox == NULL){
        return 0; // Lista vazia ou com um elemento
    }
    Lista* p;
    menor 
    for(p=lst; p!=NULL; p=p->prox){
        if(p->info == v){
            return p;
        }
    }
}