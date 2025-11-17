/*3- Escreva um programa de referência cruzada que construa uma árvore binária de pesquisa a 
partir de um arquivo de texto, incluindo todas as palavras, e registre os números das linhas em 
que  essas  palavras  foram  usadas.  Esses  números  de  linhas  devem  ser  armazenados  em  listas 
ligadas associadas aos nós da árvore. Depois do arquivo de entrada ter sido processado, imprima 
em ordem alfabética todas as palavras do arquivo de texto, junto com a lista de números de linhas 
correspondentes nas quais as palavras ocorrem. */

#include <stdio.h>
#include <stdlib.h>

typedef struct arv {
    int info;
    struct arv* esq;
    struct arv* dir;
} Arv;

Arv* abb_cria (void);
void abb_imprime(Arv* a);
Arv* abb_busca(Arv* a, int v);
Arv* abb_insere(Arv* a, int v);
Arv* abb_retira(Arv* r, int v);

Arv* abb_criar(void){
    return NULL;
}
void abb_imprime(Arv* a){
    if(a!=NULL){
        abb_imprime(a->esq);
        printf("%d ", a->info);
        abb_imprime(a->dir);
    }
}
Arv* abb_busca(Arv* r, int v){
    if(r==NULL){
        return NULL;
    }else if(v < r->info){
        return abb_busca(r->esq, v);
    }else if(v > r->info){
        return abb_busca(r->dir, v);
    }else{
        return r;
    }
}
Arv* abb_insere(Arv* a, int v){
    if(a==NULL){
        a = (Arv*) malloc (sizeof(Arv));
        a->info = v;
        a->esq = NULL;
        a->dir = NULL;
    }else if(v < a->info){
        a->esq = abb_insere(a->esq, v);
    }else if(v > a->info){
        a->dir = abb_insere(a->dir, v);
    }
    return a;
}
Arv* abb_retira(Arv* r, int v){
    if(r==NULL){
        return NULL;
    }else if(r->info > v){
        r->esq = abb_retira(r->esq, v);
    }else if(r->info < v){
        r->dir = abb_retira(r->dir, v);
    }else{ // achou o nó a ser removido
        // Nó sem filhos
        if(r->esq == NULL && r->dir == NULL){
            free(r);
            r= NULL;
        }
        // Só tem filho a direita
        else if(r->esq == NULL){
            Arv* t = r;
            r = r->dir;
            free(t);
        } 
        // Só tem filho a esquerda
        else if(r->dir == NULL){
            Arv* t = r;
            r= r->esq;
            free(t);
        }
        // Nó tem os dois filhos
        else{
            Arv* f = r->esq;
            while(f->dir != NULL){
                f = f->dir;
            }
            r->info = f->info;
            f->info = v;
            r->esq = abb_retira(r->esq, v);
        }
    }
    return r;
}