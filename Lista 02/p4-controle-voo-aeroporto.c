/*4 - Faça um programa para simular um controlador de voo de um aeroporto. Neste 
programa o usuário deve ser capaz de realizar as seguintes tarefas: 
• Listar o número de aviões esperando para decolar; 
• Autorizar a decolagem do primeiro avião na fila; 
• Adicionar um avião na fila de espera; 
• Listar todos os aviões que estão na lista de espera; 
• Listar as características do primeiro avião da fila; 
Considere que uma estrutura de dados do tipo fila seja usada para manipular os dados 
e que cada avião possui um nome, um identificador, uma origem e um destino. Se quiser 
coloque mais informações, nº de passageiros, capacidade, modelo, etc. */

#include <stdio.h>
#include <stdlib.h>

#define N 100

typedef struct aviao {
    char nome[50];
    char identificador[10];
    char origem[50];
    char destino[50];
} Aviao;

typedef struct fila {
    int n; // número de elementos armazenados na fila
    int ini; // indice do inicio da fila
    Aviao dados[N];
} Fila;

Fila* fila_cria(void);
void fila_insere(Fila* f, Aviao a);
Aviao fila_retira(Fila* f);
int fila_vazia(Fila* f);
void fila_libera(Fila* f);
// Tarefas de controle de voo
void listar_numero_avioes(Fila* f);
void autorizar_decolagem(Fila* f);
void adicionar_aviao(Fila* f);
void listar_avioes(Fila* f);
void listar_primeiro_aviao(Fila* f);


int main(void) {
    Fila* f = fila_cria();
    int opcao;

    // Loop principal do menu
    do {
        printf("\n--- ✈️  CONTROLE DE VOO ✈️ ---\n");
        printf("1. Listar número de aviões na fila\n");
        printf("2. Autorizar decolagem do próximo avião\n");
        printf("3. Adicionar avião à fila\n");
        printf("4. Listar todos os aviões na fila\n");
        printf("5. Listar características do próximo avião\n");
        printf("0. Sair do programa\n");
        printf("----------------------------------\n");
        printf("Escolha uma opção: ");

        // Lê a opção do usuário
        if (scanf("%d", &opcao) != 1) {
            // Se o usuário digitar algo que não é número
            printf("Erro: Por favor, digite um número válido.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n'); 
            opcao = -1; // Define uma opção inválida para continuar o loop
            continue;
        }

        // Limpa o buffer de entrada para remover o '\n'
        // Isso é importante para o scanf de 'adicionar_aviao' funcionar
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        // Executa a ação baseada na escolha
        switch (opcao) {
            case 1:
                listar_numero_avioes(f);
                break;
            case 2:
                autorizar_decolagem(f);
                break;
            case 3:
                adicionar_aviao(f);
                break;
            case 4:
                listar_avioes(f);
                break;
            case 5:
                listar_primeiro_aviao(f);
                break;
            case 0:
                printf("Encerrando o sistema. Bom voo!\n");
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
        }

    } while (opcao != 0); // O loop continua até o usuário digitar 0

    // Libera a memória da fila antes de sair
    fila_libera(f);
    return 0;
}

Fila* fila_cria(void) {
    Fila* f = (Fila*) malloc(sizeof(Fila));
    f->n = 0;
    f->ini = 0;
    return f;
}
void fila_insere(Fila* f, Aviao a) {
    int fim;
    if (f->n == N) { /* fila cheia */
        printf("Capacidade da fila estourou\n");
        exit(1);
    }
    /* insere elemento na proxima posicao livre*/
    fim = (f->ini + f->n) % N;
    f->dados[fim] = a;
    f->n++;
}
Aviao fila_retira(Fila* f) {
    Aviao a;
    if (fila_vazia(f)) {
        printf("Fila vazia\n");
        exit(1);
    }
    /* retira elemento do inicio da fila */
    a = f->dados[f->ini];
    f->ini = (f->ini + 1) % N;
    f->n--;
    return a;
}
int fila_vazia(Fila* f) {
    return (f->n == 0);
}
void fila_libera(Fila* f) {
    free(f);
}
void listar_numero_avioes(Fila* f) {
    printf("Número de aviões na fila: %d\n", f->n);
}
void autorizar_decolagem(Fila* f) {
    if (fila_vazia(f)) {
        printf("Nenhum avião na fila para decolar.\n");
    } else {
        Aviao a = fila_retira(f);
        printf("Avião %s autorizado a decolar.\n", a.nome);
    }
}
void adicionar_aviao(Fila* f) {
    Aviao a;
    printf("Digite o nome do avião: ");
    scanf("%s", a.nome);
    printf("Digite o identificador do avião: ");
    scanf("%s", a.identificador);
    printf("Digite a origem do avião: ");
    scanf("%s", a.origem);
    printf("Digite o destino do avião: ");
    scanf("%s", a.destino);
    fila_insere(f, a);
    printf("Avião %s adicionado à fila de espera.\n", a.nome);
}
void listar_avioes(Fila* f) {
    if (fila_vazia(f)) {
        printf("Nenhum avião na fila de espera.\n");
    } else {
        printf("Aviões na fila de espera:\n");
        for (int i = 0; i < f->n; i++) {
            int idx = (f->ini + i) % N;
            Aviao a = f->dados[idx];
            printf("Nome: %s, ID: %s, Origem: %s, Destino: %s\n", a.nome, a.identificador, a.origem, a.destino);
        }
    }
}
void listar_primeiro_aviao(Fila* f) {
    if (fila_vazia(f)) {
        printf("Nenhum avião na fila de espera.\n");
    } else {
        Aviao a = f->dados[f->ini];
        printf("Primeiro avião na fila:\n");
        printf("Nome: %s, ID: %s, Origem: %s, Destino: %s\n", a.nome, a.identificador, a.origem, a.destino);
    }
}