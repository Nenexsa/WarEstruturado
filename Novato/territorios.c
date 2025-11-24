#include <stdio.h>
#include <string.h>

/*
 * Definição da estrutura Territorio.
 * Ela armazena: nome do território, cor do exército
 * e quantidade de tropas.
 */
struct Territorio {
    char nome[30];
    char cor[10];
    int tropas;
};

int main() {

    // Vetor que armazenará os 5 territórios
    struct Territorio territorios[5];

    // Entrada de dados
    printf("=== Cadastro de Territorios ===\n\n");

    for (int i = 0; i < 5; i++) {
        printf("Territorio %d:\n", i + 1);

        // Lê o nome do território
        printf("Digite o nome: ");
        scanf("%s", territorios[i].nome);  
        // scanf para strings simples (sem espaços)

        // Lê a cor do exército
        printf("Digite a cor do exercito: ");
        scanf("%s", territorios[i].cor);

        // Lê a quantidade de tropas
        printf("Digite a quantidade de tropas: ");
        scanf("%d", &territorios[i].tropas);

        printf("\n");
    }

    // Exibição dos dados cadastrados
    printf("\n=== Territorios Cadastrados ===\n");

    for (int i = 0; i < 5; i++) {
        printf("\nTerritorio %d\n", i + 1);
        printf("Nome: %s\n", territorios[i].nome);
        printf("Cor do exercito: %s\n", territorios[i].cor);
        printf("Tropas: %d\n", territorios[i].tropas);
    }

    return 0;
}
