#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(long x) {
    if (x < 2) return 0;
    if (x == 2) return 1;
    if (x % 2 == 0) return 0; 
    long r = (long)sqrt((double)x);
    for (long i = 3; i <= r; i += 2) {
        if (x % i == 0) return 0;
    }
    return 1;
}

int main(void) {
    long n;
    printf("Digite o valor de n: ");
    if (scanf("%ld", &n) != 1 || n <= 0) {
        fprintf(stderr, "Entrada invalida. Informe um inteiro positivo.\n");
        return 1;
    }

    long count = 0;
    long num = 2;
    while (count < n) {
        if (is_prime(num)) {
            printf("%ld", num);
            count++;
            if (count < n) printf(" ");
        }
        num++;
    }
    printf("\n");
    return 0;
}
