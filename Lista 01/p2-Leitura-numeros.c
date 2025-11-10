/* 2  –  Faça  um  programa  que  leia  uma  sequencia  de  valores  inteiros  fornecidos  pelo 
usuário  em  uma  linha  de  entrada  e  conte  o  número  de  valores  positivos,  negativos  e 
zeros. Forneça também o percentual de cada um deles */

#include <stdio.h> 

int main(void) {
    int num;
    int positivos = 0, negativos = 0, zeros = 0;
    int total = 0;

    printf("Digite uma sequencia de numeros inteiros (pressione Ctrl+D para terminar):\n");

    while (scanf("%d", &num) == 1) {
        total++;
        if (num > 0) {
            positivos++;
        } else if (num < 0) {
            negativos++;
        } else {
            zeros++;
        }
    }

    if (total == 0) {
        printf("Nenhum numero foi fornecido.\n");
        return 0;
    }

    printf("Positivos: %d (%.2f%%)\n", positivos, (positivos * 100.0) / total);
    printf("Negativos: %d (%.2f%%)\n", negativos, (negativos * 100.0) / total);
    printf("Zeros: %d (%.2f%%)\n", zeros, (zeros * 100.0) / total);

    return 0;
}

