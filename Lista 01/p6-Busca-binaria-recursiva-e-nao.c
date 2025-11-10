/*6 – Escreva o algoritmo de busca binária (na forma recursiva 
e não recursiva) e faça a análise de tempo de execução do 
pior caso de cada algoritmo. 
*/
#include <stdio.h>
#include <stdlib.h>

int busca_binaria_recursiva(int arr[], int esquerda, int direita, int alvo) {
    if (direita >= esquerda) {
        int meio = esquerda + (direita - esquerda) / 2;
        if (arr[meio] == alvo) {
            return meio;
        }
        if (arr[meio] > alvo) {
            return busca_binaria_recursiva(arr, esquerda, meio - 1, alvo);
        } else {
            return busca_binaria_recursiva(arr, meio + 1, direita, alvo);
        }  
    }
    return -1; // Elemento não encontrado
}

int busca_binaria_iterativa(int arr[], int tamanho, int alvo) 
{
    int esquerda = 0;
    int direita = tamanho - 1;

    while (esquerda <= direita) 
    {
        int meio = esquerda + (direita - esquerda) / 2;
        if (arr[meio] == alvo) 
        {
            return meio;
        }
        if (arr[meio] < alvo) 
        {
            esquerda = meio + 1;
        }    
        else 
        {
            direita = meio - 1;
        }
    }
    return -1; // Elemento não encontrado
}

int main() 
{
    int arr[] = {2, 3, 10, 40, 50};
    int tamanho = sizeof(arr) / sizeof(arr[0]);
    int alvo = 40;

    // Busca binária recursiva
    int resultado_recursivo = busca_binaria_recursiva(arr, 0, tamanho - 1, alvo);
    if (resultado_recursivo != -1)
        printf("Elemento encontrado na posição (recursivo): %d\n", resultado_recursivo);
    else
        printf("Elemento não encontrado (recursivo)\n");

    // Busca binária iterativa
    int resultado_iterativo = busca_binaria_iterativa(arr, tamanho, alvo);
    if (resultado_iterativo != -1) 
    {
        printf("Elemento encontrado na posição (iterativo): %d\n", resultado_iterativo);
        printf("--------------------------------\n");
    }
    else 
    {
        printf("Elemento não encontrado (iterativo)\n");
        printf("--------------------------------\n");
        return -1; // Elemento não encontrado
    }    
    return 0;
}
