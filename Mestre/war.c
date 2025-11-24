#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* --- Estruturas --- */

/*  Estrutura do Territorio */
typedef struct Territorio {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

/* Estrutura do Jogador */
typedef struct Jogador {
    char cor[10];
    char *missao;              
    int missao_exibida;       
} Jogador;

/* --- Variáveis globais --- */

/* Ponteiro  para o mapa*/
static Territorio *g_mapa_global = NULL;
static int g_mapa_tamanho = 0;

/* Vetor de jogadores */
static Jogador *jogadores = NULL;
static int numJogadores = 0;

/* Missões disponíveis */
static char *missoesDisponiveis[] = {
    "Conquistar 3 territorios",
    "Conquistar 5 territorios",
    "Eliminar todas as tropas da cor vermelha",
    "Ter mais tropas totais que qualquer outro jogador",
    "Controlar todos os territorios"
};
static int totalMissoesDisponiveis = sizeof(missoesDisponiveis) / sizeof(missoesDisponiveis[0]);


void limparEntrada(void);
void cadastrarTerritorios(Territorio *mapa, int n);
void exibirTerritorios(Territorio *mapa, int n);
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMemoria(Territorio *mapa);
int validarIndice(int idx, int n);
void menuAtaques(Territorio *mapa, int n);

void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int verificarMissao(char* missao, Territorio* mapa, int tamanho);
void exibirMissao(const char* missao);
void criarJogadoresAPartirDoMapa(Territorio* mapa, int n);
void liberarJogadores(void);



/* Limpa a entrada */
void limparEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* Cadastro de territorios */
void cadastrarTerritorios(Territorio *mapa, int n) {
    for (int i = 0; i < n; i++) {
        printf("Territorio %d:\n", i + 1);

        printf("Digite o nome: ");
        if (fgets((mapa + i)->nome, sizeof((mapa + i)->nome), stdin) == NULL) {
            (mapa + i)->nome[0] = '\0';
        } else {
            (mapa + i)->nome[strcspn((mapa + i)->nome, "\n")] = '\0';
        }

        printf("Digite a cor do exercito: ");
        if (fgets((mapa + i)->cor, sizeof((mapa + i)->cor), stdin) == NULL) {
            (mapa + i)->cor[0] = '\0';
        } else {
            (mapa + i)->cor[strcspn((mapa + i)->cor, "\n")] = '\0';
        }

        printf("Digite a quantidade de tropas: ");
        while (scanf("%d", &((mapa + i)->tropas)) != 1) {
            
            limparEntrada();
            printf("Entrada inválida. Digite um número inteiro para tropas: ");
        }
       
        limparEntrada();

        printf("\n");
    }

    /* Atribuicao de cor aos jogadores */
    criarJogadoresAPartirDoMapa(mapa, n);

    /* Exibir as missões apenas uma vez (após o cadastro) */
    if (numJogadores > 0) {
        printf("\n=== Missões atribuídas aos jogadores ===\n");
        for (int j = 0; j < numJogadores; j++) {
            exibirMissao(jogadores[j].missao);
            jogadores[j].missao_exibida = 1;
        }
        printf("=======================================\n\n");
    }
}

/* Exibe todos os territórios do mapa */
void exibirTerritorios(Territorio *mapa, int n) {
    printf("\n=== Territorios Cadastrados ===\n");
    for (int i = 0; i < n; i++) {
        printf("\nTerritorio %d\n", i + 1);
        printf("Nome: %s\n", (mapa + i)->nome);
        printf("Cor do exercito: %s\n", (mapa + i)->cor);
        printf("Tropas: %d\n", (mapa + i)->tropas);
    }
}

/* Rolagem dos dados */
void atacar(Territorio *atacante, Territorio *defensor) {
    if (atacante == NULL || defensor == NULL) return;

    printf("\n--- Simulando ataque ---\n");
    printf("Atacante: %s (%s) - Tropas: %d\n", atacante->nome, atacante->cor, atacante->tropas);
    printf("Defensor : %s (%s) - Tropas: %d\n", defensor->nome, defensor->cor, defensor->tropas);

    if (atacante->tropas <= 0) {
        printf("Ataque inválido: o território atacante não tem tropas suficientes.\n");
        return;
    }

    /* rolagem dos dados */
    int dado_atacante = rand() % 6 + 1;
    int dado_defensor  = rand() % 6 + 1;

    printf("Dado do atacante: %d\n", dado_atacante);
    printf("Dado do defensor : %d\n", dado_defensor);

    if (dado_atacante > dado_defensor) {
        /* atacante vence: transfere cor e metade das tropas */
        int transf = atacante->tropas / 2;
        if (transf < 1) transf = 1; /* garante ao menos 1 tropa transferida se houver tropas */

        /* copia a cor */
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor)-1);
        defensor->cor[sizeof(defensor->cor)-1] = '\0';

        /* ajuste de tropas */
        defensor->tropas = transf;
        atacante->tropas -= transf;
        if (atacante->tropas < 0) atacante->tropas = 0;

        printf("Atacante venceu! Territorio '%s' agora pertence a '%s' com %d tropas.\n",
               defensor->nome, defensor->cor, defensor->tropas);
        printf("Tropas restantes do atacante '%s': %d\n", atacante->nome, atacante->tropas);
    } else {
        /* atacante perde: perde 1 tropa */
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
        printf("Atacante perdeu (ou empatou). '%s' perde 1 tropa. Tropas restantes: %d\n",
               atacante->nome, atacante->tropas);
    }
}

/* Libera memória alocada */
void liberarMemoria(Territorio *mapa) {
    if (mapa != NULL) {
        free(mapa);
        g_mapa_global = NULL;
        g_mapa_tamanho = 0;
    }
    liberarJogadores();
}


int validarIndice(int idx, int n) {
    return (idx >= 0 && idx < n);
}

/*Menu de ataques */
void menuAtaques(Territorio *mapa, int n) {
    /* atualiza globals para que verificarMissao acesse o mapa */
    g_mapa_global = mapa;
    g_mapa_tamanho = n;

    while (1) {
        exibirTerritorios(mapa, n);
        printf("\nMenu:\n");
        printf("1 - Realizar ataque\n");
        printf("2 - Sair\n");
        printf("Escolha uma opcao: ");

        int opc;
        if (scanf("%d", &opc) != 1) {
            /* limpar entrada inválida */
            limparEntrada();
            printf("Entrada inválida.\n");
            continue;
        }
        limparEntrada(); /* limpa buffer */

        if (opc == 1) {
            int ia, id;
            printf("Digite o numero do territorio atacante (1..%d): ", n);
            if (scanf("%d", &ia) != 1) { limparEntrada(); printf("Entrada inválida.\n"); continue; }
            printf("Digite o numero do territorio defensor (1..%d): ", n);
            if (scanf("%d", &id) != 1) { limparEntrada(); printf("Entrada inválida.\n"); continue; }
            limparEntrada(); /* limpa buffer */

            ia--; id--; /* transforma para 0-based */
            if (!validarIndice(ia, n) || !validarIndice(id, n)) {
                printf("Índices fora do intervalo. Tente novamente.\n");
                continue;
            }
            if (ia == id) {
                printf("Um territorio não pode atacar ele mesmo. Tente novamente.\n");
                continue;
            }
            /* valida cores diferentes (não pode atacar mesma cor) */
            if (strcmp((mapa + ia)->cor, (mapa + id)->cor) == 0) {
                printf("Ataque inválido: não é possível atacar um território da mesma cor.\n");
                continue;
            }

            /* realiza ataque */
            atacar((mapa + ia), (mapa + id));

            /* após ataque, verificar missões de todos os jogadores usando o mapa global */
            for (int j = 0; j < numJogadores; j++) {
                if (jogadores[j].missao != NULL) {
                    if (verificarMissao(jogadores[j].missao, g_mapa_global, g_mapa_tamanho)) {
                        /* extrair a cor do jogador da string "cor:descricao" para exibir */
                        char cor_aux[10] = {0};
                        const char *sep = strchr(jogadores[j].missao, ':');
                        if (sep != NULL) {
                            size_t len = sep - jogadores[j].missao;
                            if (len >= sizeof(cor_aux)) len = sizeof(cor_aux)-1;
                            strncpy(cor_aux, jogadores[j].missao, len);
                            cor_aux[len] = '\0';
                        } else {
                            strncpy(cor_aux, "Desconhecida", sizeof(cor_aux)-1);
                        }
                        printf("\n*** MISSÃO CUMPRIDA! JOGADOR '%s' VENCEU! ***\n", cor_aux);
                        printf("Missão: %s\n", sep ? (sep+1) : jogadores[j].missao);

                        /* libera memória e finaliza programa */
                        liberarMemoria(g_mapa_global);
                        printf("\nMemoria liberada. Programa finalizado.\n");
                        exit(0);
                    }
                }
            }
        } else if (opc == 2) {
            printf("Saindo do menu de ataques.\n");
            break;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }
}

/* ----------------------- Funções de missões e jogadores ----------------------- */

/* Sorteia uma missão */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    if (destino == NULL || missoes == NULL || totalMissoes <= 0) return;
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

/* Exibe a missão  */
void exibirMissao(const char* missao) {
    if (missao == NULL) return;
    const char *sep = strchr(missao, ':');
    if (sep != NULL) {
        printf("Jogador '%.*s' recebeu a missao: %s\n", (int)(sep - missao), missao, sep+1);
    } else {
        printf("Missao: %s\n", missao);
    }
}

/* Cria jogadores a partir das cores encontradas no mapa.*/
void criarJogadoresAPartirDoMapa(Territorio* mapa, int n) {
    if (mapa == NULL || n <= 0) return;

    /* identifica cores únicas  */
    char coresUnicas[50][10];
    int contadorCores = 0;

    for (int i = 0; i < n; i++) {
        int achou = 0;
        for (int j = 0; j < contadorCores; j++) {
            if (strcmp(mapa[i].cor, coresUnicas[j]) == 0) { achou = 1; break; }
        }
        if (!achou && contadorCores < 50) {
            strncpy(coresUnicas[contadorCores], mapa[i].cor, sizeof(coresUnicas[contadorCores]) - 1);
            coresUnicas[contadorCores][sizeof(coresUnicas[contadorCores]) - 1] = '\0';
            contadorCores++;
        }
    }

    /* liberar jogadores anteriores, se houver */
    liberarJogadores();

    /* alocar vetor de jogadores */
    numJogadores = contadorCores;
    if (numJogadores <= 0) return;

    jogadores = (Jogador *) calloc((size_t)numJogadores, sizeof(Jogador));
    if (jogadores == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para jogadores.\n");
        numJogadores = 0;
        return;
    }

    /* atribuir cor e sortear missões (armazenadas dinamicamente) */
    for (int k = 0; k < numJogadores; k++) {
        strncpy(jogadores[k].cor, coresUnicas[k], sizeof(jogadores[k].cor)-1);
        jogadores[k].cor[sizeof(jogadores[k].cor)-1] = '\0';
        jogadores[k].missao_exibida = 0;

        /* compor "cor:descricao" */
        const char *descEscolhida = missoesDisponiveis[rand() % totalMissoesDisponiveis];
        size_t tam = strlen(jogadores[k].cor) + 1 + strlen(descEscolhida) + 1; /* cor + ':' + desc + '\0' */
        jogadores[k].missao = (char *) malloc(tam);
        if (jogadores[k].missao != NULL) {
            snprintf(jogadores[k].missao, tam, "%s:%s", jogadores[k].cor, descEscolhida);
        } else {
            jogadores[k].missao = NULL;
        }
    }
}

/* Libera memória alocada para jogadores e suas missões */
void liberarJogadores(void) {
    if (jogadores != NULL) {
        for (int i = 0; i < numJogadores; i++) {
            if (jogadores[i].missao != NULL) {
                free(jogadores[i].missao);
                jogadores[i].missao = NULL;
            }
        }
        free(jogadores);
        jogadores = NULL;
        numJogadores = 0;
    }
}


 /*Verifica a conclusao da missao*/
int verificarMissao(char* missao, Territorio* mapa, int tamanho) {
    if (missao == NULL) return 0;

    /* usa globals se necessário */
    if (mapa == NULL || tamanho <= 0) {
        mapa = g_mapa_global;
        tamanho = g_mapa_tamanho;
    }
    if (mapa == NULL || tamanho <= 0) return 0;

    /* extrair cor e descricao (se houver) */
    char corJogador[10] = {0};
    char descricao[200] = {0};
    const char *sep = strchr(missao, ':');
    if (sep == NULL) {
        strncpy(descricao, missao, sizeof(descricao)-1);
    } else {
        size_t lenCor = sep - missao;
        if (lenCor >= sizeof(corJogador)) lenCor = sizeof(corJogador)-1;
        strncpy(corJogador, missao, lenCor);
        corJogador[lenCor] = '\0';
        strncpy(descricao, sep+1, sizeof(descricao)-1);
        descricao[sizeof(descricao)-1] = '\0';
    }

    /* contar territorios e tropas do jogador e acumular tropas por cor */
    int territoriosDoJogador = 0;
    char coresEncontradas[50][10];
    int somaTropasPorCor[50];
    int numCoresEncontradas = 0;

    for (int i = 0; i < tamanho; i++) {
        /* contar territorios do jogador */
        if (strlen(corJogador) > 0 && strcmp(mapa[i].cor, corJogador) == 0) {
            territoriosDoJogador++;
        }

        /* acumular tropas por cor */
        int idx = -1;
        for (int c = 0; c < numCoresEncontradas; c++) {
            if (strcmp(coresEncontradas[c], mapa[i].cor) == 0) { idx = c; break; }
        }
        if (idx == -1) {
            if (numCoresEncontradas < 50) {
                idx = numCoresEncontradas++;
                strncpy(coresEncontradas[idx], mapa[i].cor, sizeof(coresEncontradas[idx])-1);
                coresEncontradas[idx][sizeof(coresEncontradas[idx])-1] = '\0';
                somaTropasPorCor[idx] = mapa[i].tropas;
            } else {
                /* fallback (não esperado para este exercício) */
                idx = 0;
                somaTropasPorCor[idx] += mapa[i].tropas;
            }
        } else {
            somaTropasPorCor[idx] += mapa[i].tropas;
        }
    }

    /* Verificações conforme a descrição da missão */
    if (strstr(descricao, "Conquistar 3 territorios") != NULL) {
        return (territoriosDoJogador >= 3) ? 1 : 0;
    }
    if (strstr(descricao, "Conquistar 5 territorios") != NULL) {
        return (territoriosDoJogador >= 5) ? 1 : 0;
    }
    if (strstr(descricao, "Eliminar todas as tropas da cor") != NULL) {
        /* tenta extrair a cor alvo na descrição (última palavra) */
        char corAlvo[10] = {0};
        const char *p = strrchr(descricao, ' ');
        if (p != NULL) {
            strncpy(corAlvo, p+1, sizeof(corAlvo)-1);
        } else {
            strncpy(corAlvo, "vermelha", sizeof(corAlvo)-1);
        }
        /* soma tropas da cor alvo */
        int tropasAlvo = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corAlvo) == 0) tropasAlvo += mapa[i].tropas;
        }
        return (tropasAlvo == 0) ? 1 : 0;
    }
    if (strstr(descricao, "Ter mais tropas totais que qualquer outro jogador") != NULL) {
        /* encontrar índice da cor do jogador nas cores encontradas */
        int idxJog = -1;
        for (int c = 0; c < numCoresEncontradas; c++) {
            if (strcmp(coresEncontradas[c], corJogador) == 0) { idxJog = c; break; }
        }
        if (idxJog == -1) return 0; /* jogador não possui territorios */

        int tropasJog = somaTropasPorCor[idxJog];
        for (int c = 0; c < numCoresEncontradas; c++) {
            if (c == idxJog) continue;
            if (tropasJog <= somaTropasPorCor[c]) return 0;
        }
        return 1;
    }
    if (strstr(descricao, "Controlar todos os territorios") != NULL) {
        /* verifica se todos os territorios têm a cor do jogador */
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) != 0) return 0;
        }
        return 1;
    }

    /* missão não reconhecida -> não cumprida */
    return 0;
}


int main(void) {
    srand((unsigned int)time(NULL));

    printf("=== War Estruturado - Módulo de Ataque (arquivo separado) ===\n");
    printf("Quantos territorios deseja cadastrar? ");
    int n;
    while (scanf("%d", &n) != 1 || n <= 0) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        printf("Entrada inválida. Digite um numero inteiro positivo para a quantidade de territorios: ");
    }
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {} // limpa buffer

    // aloca dinamicamente o vetor de territorios
    Territorio *mapa = (Territorio *) calloc((size_t)n, sizeof(Territorio));
    if (mapa == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para os territorios.\n");
        return 1;
    }

    // cadastro
    cadastrarTerritorios(mapa, n);

    // menu interativo de ataques
    menuAtaques(mapa, n);

    // exibe estado final
    exibirTerritorios(mapa, n);

    // libera memória
    liberarMemoria(mapa);

    printf("\nMemoria liberada. Programa finalizado.\n");
    return 0;
}
