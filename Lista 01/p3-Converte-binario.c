/* 3 – Faça um programa que converta um número inteiro sem sinal 
na base 2 (sequencia de 16 valores de 0 ́s e 1 ́s), e converta para a base 10. 
Faça um programa para realizar a operação inversa também. */

#include <stdio.h>
#include <string.h>
#include <math.h>

// variáveis globais para manter o valor decimal e a representação binária (16 bits)
int g_decimal = 0;
int g_binario[16] = {0};

// Função agora usa a variável global g_decimal e preenche g_binario
void decimalParaBinario(void) {
    int num = g_decimal;
    for (int i = 15; i >= 0; i--) {
        g_binario[i] = num % 2;
        num /= 2;
    }
    printf("Representação em binário: ");
    for (int i = 0; i < 16; i++) {
        printf("%d", g_binario[i]);
    }
    printf("\n");
}

// Função agora usa a variável global g_binario e retorna o decimal
int binarioParaDecimal(void) {
    int decimal = 0;
    for (int i = 0; i < 16; i++) {
        decimal = (decimal << 1) | (g_binario[i] & 1); // assume bits[0] = MSB, bits[15] = LSB
    }
    g_decimal = decimal;
    return decimal;
}


int main(void) {
    int escolha, numero;

    printf("Escolha a conversão:\n");
    printf("1 - Decimal para Binário\n");
    printf("2 - Binário para Decimal\n");
    printf("Digite sua escolha (1 ou 2): ");
    if (scanf("%d", &escolha) != 1) {
        printf("Entrada inválida.\n");
        return 1;
    }

    if (escolha == 1) {
        printf("Digite um número inteiro sem sinal (0 a 65535): ");
        if (scanf("%d", &numero) != 1) {
            printf("Entrada inválida.\n");
            return 1;
        }
        if (numero < 0 || numero > 65535) {
            printf("Número fora do intervalo permitido.\n");
            return 1;
        }
        g_decimal = numero;           // usa a variável global
        decimalParaBinario();         // sem parâmetro
    } else if (escolha == 2) {
        char buf[32];
        printf("Digite um número binário (até 16 bits). Use apenas 0 e 1: ");
        if (scanf("%31s", buf) != 1) {
            printf("Entrada inválida.\n");
            return 1;
        }

        size_t len = strlen(buf);
        if (len == 0 || len > 16) {
            printf("Número binário deve ter entre 1 e 16 bits.\n");
            return 1;
        }

        // validar e preencher g_binario alinhado à direita (LSB em g_binario[15])
        for (size_t i = 0; i < 16; i++) g_binario[i] = 0; // limpar
        for (size_t i = 0; i < len; i++) {
            char c = buf[i];
            if (c != '0' && c != '1') {
                printf("Entrada inválida. Use apenas 0 ou 1.\n");
                return 1;
            }
            // coloca o bit no lugar correto: primeiro caractere -> posição (16 - len)
            g_binario[16 - len + i] = c - '0';
        }

        int resultado = binarioParaDecimal(); // sem parâmetro
        printf("Representação em decimal: %d\n", resultado);
    } else {
        printf("Escolha inválida.\n");
    }
    return 0;
}

