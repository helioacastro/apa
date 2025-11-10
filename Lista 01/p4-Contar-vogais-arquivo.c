/*
4 – Faça um programa que leia um texto do usuário e conte o número 
de vogais  que aparecem. O texto fornecido deve estar em um arquivo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int contarVogais(const char *texto) {
    int contador = 0;
    for (int i = 0; texto[i] != '\0'; i++) {
        char c = tolower(texto[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            contador++;
        }
    }
    return contador;
}

int main() {
    char nomeArquivo[100];
    printf("Digite o nome do arquivo de texto: ");
    scanf("%s", nomeArquivo);

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    char *texto = malloc(tamanho + 1);
    if (texto == NULL) {
        printf("Erro ao alocar memoria.\n");
        fclose(arquivo);
        return 1;
    }

    fread(texto, 1, tamanho, arquivo);
    texto[tamanho] = '\0';

    fclose(arquivo);

    int numeroVogais = contarVogais(texto);
    printf("Numero de vogais no texto: %d\n", numeroVogais);

    free(texto);
    return 0;
}