/*3 - Desenvolva um método para manter duas pilhas dentro de um único vetor linear (um 
arranjo) de modo que nenhuma das pilhas incorra em estouro até que toda a memória 
seja usada, e toda uma pilha nunca seja deslocada para outro local dentro do vetor.
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct duas_pilhas {
    int dados[MAX];
    int topo1;
    int topo2;
} DuasPilhas;

DuasPilhas* cria_duas_pilhas();
int push1(DuasPilhas* dp, int val);
int push2(DuasPilhas* dp, int val);
int pop1(DuasPilhas* dp);
int pop2(DuasPilhas* dp);
int pilhas_vazias(DuasPilhas* dp);
void libera_duas_pilhas(DuasPilhas* dp);

int main(void) {
    DuasPilhas* dp = cria_duas_pilhas();

    push1(dp, 10);
    push1(dp, 20);
    push2(dp, 30);
    push2(dp, 40);

    printf("Pop da pilha 1: %d\n", pop1(dp));
    printf("Pop da pilha 2: %d\n", pop2(dp));

    libera_duas_pilhas(dp);
    return 0;
}

DuasPilhas* cria_duas_pilhas() {
    DuasPilhas* dp = (DuasPilhas*) malloc(sizeof(DuasPilhas));
    dp->topo1 = -1;
    dp->topo2 = MAX;
    return dp;
}
int push1(DuasPilhas* dp, int val) {
    if (dp->topo1 + 1 == dp->topo2) {
        printf("Erro: Pilha 1 cheia\n");
        return 0;
    }
    dp->dados[++dp->topo1] = val;
    return 1;
}
int push2(DuasPilhas* dp, int val) {
    if (dp->topo2 - 1 == dp->topo1) {
        printf("Erro: Pilha 2 cheia\n");
        return 0;
    }
    dp->dados[--dp->topo2] = val;
    return 1;
}
int pop1(DuasPilhas* dp) {
    if (dp->topo1 == -1) {
        printf("Erro: Pilha 1 vazia\n");
        exit(1);
    }
    return dp->dados[dp->topo1--];
}
int pop2(DuasPilhas* dp) {
    if (dp->topo2 == MAX) {
        printf("Erro: Pilha 2 vazia\n");
        exit(1);
    }
    return dp->dados[dp->topo2++];
}
void libera_duas_pilhas(DuasPilhas* dp) {
    free(dp);
}
int pilhas_vazias(DuasPilhas* dp) {
    return (dp->topo1 == -1) && (dp->topo2 == MAX);
}
