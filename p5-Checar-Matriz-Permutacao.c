/*
5 – Escrever uma função (e um programa que execute tal função) 
que determine se uma matriz  quadrada  de  dimensão  n(n<100)  
é  uma  matriz  de  permutação.  Uma  matriz quadrada é 
chamada de matriz de permutação se seus elementos são apenas
0’s e 1’s e se em cada linha e coluna da matriz existe 
apena um único valor 1.  
Exemplo:  A matriz   (
1 0 0
0 0 1
0 1 0
) é uma matriz permutação.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

int ehMatrizPermutacao(int matriz[MAX][MAX], int n) {
    for (int i = 0; i < n; i++) {
        int somaLinha = 0;
        int somaColuna = 0;
        for (int j = 0; j < n; j++) {
            somaLinha += matriz[i][j];
            somaColuna += matriz[j][i];
            if (matriz[i][j] != 0 && matriz[i][j] != 1) {
                return 0; // Elemento diferente de 0 ou 1
            }
        }
        if (somaLinha != 1 || somaColuna != 1) {
            return 0; // Linha ou coluna não tem exatamente um 1
        }
    }
    return 1; // É uma matriz de permutação
}

// Gera uma matriz n x n com valores 0 ou 1 (n deve ser 1..MAX)
void gerarMatrizAleatoria(int matriz[MAX][MAX], int n) {
    if (n <= 0 || n > MAX) return;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matriz[i][j] = rand() & 1; // 0 ou 1
        }
    }
}

// Imprime a matriz no formato 0/1
void imprimirMatriz(int matriz[MAX][MAX], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Exemplo de uso: gera matriz aleatória, imprime e verifica permutação
int main(void) {
    int n;
    printf("Digite a dimensão n (1-%d): ", MAX);
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX) {
        fprintf(stderr, "Valor de n inválido.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    int matriz[MAX][MAX];
    gerarMatrizAleatoria(matriz, n);

    printf("Matriz gerada:\n");
    imprimirMatriz(matriz, n);

    if (ehMatrizPermutacao(matriz, n)) {
        printf("É uma matriz de permutação.\n");
    } else {
        printf("Não é uma matriz de permutação.\n");
    }

    return 0;
}






