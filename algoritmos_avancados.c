/*
 Desafio Detective Quest - Níveis Novato, Aventureiro e Mestre
 Autor: Enigma Studios (exemplo)
 - Árvore binária para mapa de salas (Novato)
 - Árvore de busca (BST) para pistas (Aventureiro)
 - Tabela hash para associar pistas -> suspeitos + contagem (Mestre)

 Compilar: gcc -std=c11 -Wall -Wextra -o desafio4_detective desafio4_detective.c
 Executar: ./desafio4_detective
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------- Estrutura das Salas (Árvore Binária) ----------------------------- */
typedef struct Room {
    char *name;
    char *clue;            // NULL se sala não contem pista
    struct Room *left;
    struct Room *right;
} Room;

Room* criarSala(const char* name, const char* clue, Room* left, Room* right) {
    Room* r = (Room*)malloc(sizeof(Room));
    r->name = strdup(name);
    r->clue = clue ? strdup(clue) : NULL;
    r->left = left;
    r->right = right;
    return r;
}

void liberarSalas(Room* root) {
    if (!root) return;
    liberarSalas(root->left);
    liberarSalas(root->right);
    if (root->name) free(root->name);
    if (root->clue) free(root->clue);
    free(root);
}

/* ----------------------------- Árvore de Busca para Pistas (BST) ----------------------------- */
typedef struct BSTNode {
    char *key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

BSTNode* bstInsert(BSTNode* root, const char* key) {
    if (!root) {
        BSTNode* n = (BSTNode*)malloc(sizeof(BSTNode));
        n->key = strdup(key);
        n->left = n->right = NULL;
        return n;
    }
    int cmp = strcmp(key, root->key);
    if (cmp < 0) root->left = bstInsert(root->left, key);
    else if (cmp > 0) root->right = bstInsert(root->right, key);
    // se igual, não insere duplicata
    return root;
}

void bstInOrder(BSTNode* root) {
    if (!root) return;
    bstInOrder(root->left);
    printf(" - %s\n", root->key);
    bstInOrder(root->right);
}

void bstFree(BSTNode* root) {
    if (!root) return;
    bstFree(root->left);
    bstFree(root->right);
    free(root->key);
    free(root);
}

/* ----------------------------- Tabela Hash para Pista -> Suspeito ----------------------------- */
#define HASH_SIZE 13

typedef struct HashEntry {
    char *clue;
    char *suspect;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry* buckets[HASH_SIZE];
} HashTable;

unsigned int simpleHash(const char* s) {
    // soma ASCII simples, modulo HASH_SIZE
    unsigned int sum = 0;
    for (; *s; ++s) sum = sum * 31 + (unsigned char)(*s);
    return sum % HASH_SIZE;
}

void hashInit(HashTable* ht) {
    for (int i = 0; i < HASH_SIZE; ++i) ht->buckets[i] = NULL;
}

void hashInsert(HashTable* ht, const char* clue, const char* suspect) {
    unsigned int h = simpleHash(clue);
    HashEntry* e = (HashEntry*)malloc(sizeof(HashEntry));
    e->clue = strdup(clue);
    e->suspect = strdup(suspect);
    e->next = ht->buckets[h];
    ht->buckets[h] = e;
}

void hashPrintAll(HashTable* ht) {
    printf("Associações pista -> suspeito (tabela hash):\n");
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry* e = ht->buckets[i];
        if (!e) continue;
        printf("Bucket %d:\n", i);
        while (e) {
            printf("  \"%s\" -> %s\n", e->clue, e->suspect);
            e = e->next;
        }
    }
}

void hashFree(HashTable* ht) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashEntry* e = ht->buckets[i];
        while (e) {
            HashEntry* nxt = e->next;
            free(e->clue);
            free(e->suspect);
            free(e);
            e = nxt;
        }
        ht->buckets[i] = NULL;
    }
}

/* ----------------------------- Suspeitos e contagem ----------------------------- */
typedef struct {
    char *name;
    int count;
} Suspect;

Suspect* createSuspectList(int n, const char** names) {
    Suspect* arr = (Suspect*)malloc(sizeof(Suspect) * n);
    for (int i = 0; i < n; ++i) {
        arr[i].name = strdup(names[i]);
        arr[i].count = 0;
    }
    return arr;
}

void freeSuspectList(Suspect* arr, int n) {
    for (int i = 0; i < n; ++i) free(arr[i].name);
    free(arr);
}

int suspectIndexByName(Suspect* arr, int n, const char* name) {
    for (int i = 0; i < n; ++i) if (strcmp(arr[i].name, name) == 0) return i;
    return -1;
}

/* ----------------------------- Mansão: construção com pistas pré-definidas ----------------------------- */
Room* construirMansao() {
    // Definição das salas (exemplo)
    // Folhas
    Room* adega = criarSala("Adega", "pegada lama", NULL, NULL);
    Room* sotao = criarSala("Sotão", "fio de seda", NULL, NULL);
    Room* escritorio = criarSala("Escritório", "bilhete rasgado", NULL, NULL);
    Room* cozinha = criarSala("Cozinha", "talher sujo", NULL, NULL);
    Room* salaJantar = criarSala("Sala de Jantar", NULL, NULL);
    Room* biblioteca = criarSala("Biblioteca", "marca de prensa", NULL, NULL);
    Room* jardim = criarSala("Jardim", "pó dourado", NULL, NULL);
    Room* quarto = criarSala("Quarto", NULL, NULL);

    // Nível intermediário
    Room* corredor = criarSala("Corredor", NULL, biblioteca, cozinha);
    Room* salaEstar = criarSala("Sala de Estar", "cartao de visita", salaJantar, escritorio);
    Room* vestibulo = criarSala("Vestíbulo", NULL, corredor, jardim);

    // Topo
    Room* hall = criarSala("Hall de Entrada", NULL, salaEstar, vestibulo);

    // adicionar mais ligações para aumentar árvore
    salaJantar->left = adega;   // exemplo ligações extras
    jardim->right = sotao;
    quarto->left = NULL; // não usado em árvore principal, mas reservado

    return hall;
}

/* ----------------------------- Percursos / Exploração ----------------------------- */

/* Simula uma exploração predefinida para Novato (não pede input do usuário) */
void simularExploracaoNovato(Room* root) {
    printf("=== Nível Novato: Navegação (simulada) pela mansão ===\n");
    if (!root) return;
    Room* cur = root;
    // sequência pré-definida de decisão: e (left), d (right), e, ... (como exemplo)
    // Para manter previsível, iremos seguir primeiro esquerda, depois direita, etc., até folha
    while (cur) {
        printf("Você entra em: %s\n", cur->name);
        if (!cur->left && !cur->right) {
            printf(" -> Nó-folha alcançado: fim do caminho.\n");
            break;
        }
        // preferência: se existe esquerda, vá para esquerda; senão vá para direita
        if (cur->left) {
            printf("  escolha automática: ir para a ESQUERDA\n\n");
            cur = cur->left;
        } else {
            printf("  escolha automática: ir para a DIREITA\n\n");
            cur = cur->right;
        }
    }
    printf("=== Fim da exploração Novato ===\n\n");
}

/* Percorre a árvore inteira em pré-ordem, coletando pistas encontradas (BST) e mapeando para suspeitos (hash) */
void coletarPistasEAssociar(Room* root, BSTNode** pistaBST, HashTable* ht, Suspect* suspects, int suspectCount, const char* (*mapPistaParaSuspeito)(const char*)) {
    if (!root) return;
    if (root->clue) {
        printf("Encontrada pista na sala \"%s\": \"%s\"\n", root->name, root->clue);
        // inserir na BST (evita duplicatas por design do bstInsert)
        *pistaBST = bstInsert(*pistaBST, root->clue);
        // consultar mapeamento pista -> suspeito (função pré-definida retorna nome do suspeito)
        const char* suspeito = mapPistaParaSuspeito(root->clue);
        if (suspeito) {
            hashInsert(ht, root->clue, suspeito);
            int idx = suspectIndexByName(suspects, suspectCount, suspeito);
            if (idx >= 0) suspects[idx].count++;
        }
    }
    coletarPistasEAssociar(root->left, pistaBST, ht, suspects, suspectCount, mapPistaParaSuspeito);
    coletarPistasEAssociar(root->right, pistaBST, ht, suspects, suspectCount, mapPistaParaSuspeito);
}

/* ----------------------------- Mapeamento pré-definido pista -> suspeito ----------------------------- */
/*
   Aqui definimos as associações "pré-jogo" que ligam cada pista a um suspeito.
   Em um jogo real isso poderia ser dinâmico, mas o enunciado pediu pré-definido.
*/
const char* mapaPistaParaSuspeito(const char* pista) {
    // mapa simples por strcmp; retornar nome do suspeito
    if (strcmp(pista, "pegada lama") == 0) return "Joaquim";
    if (strcmp(pista, "fio de seda") == 0) return "Mariana";
    if (strcmp(pista, "bilhete rasgado") == 0) return "Carlos";
    if (strcmp(pista, "talher sujo") == 0) return "Mariana";
    if (strcmp(pista, "cartao de visita") == 0) return "Fernanda";
    if (strcmp(pista, "marca de prensa") == 0) return "Joaquim";
    if (strcmp(pista, "pó dourado") == 0) return "Carlos";
    // default: se pista desconhecida, retorna NULL (sem suspeito)
    return NULL;
}

/* ----------------------------- Função utilitária para achar suspeito mais citado ----------------------------- */
void imprimirSuspeitoMaisCitado(Suspect* suspects, int n) {
    int best = -1;
    for (int i = 0; i < n; ++i) {
        if (best == -1 || suspects[i].count > suspects[best].count) best = i;
    }
    if (best == -1) {
        printf("Nenhum suspeito teve citações.\n");
    } else {
        printf("\nSuspeito mais citado: %s (citado %d vezes)\n", suspects[best].name, suspects[best].count);
    }
}

/* ----------------------------- Fluxos dos Níveis ----------------------------- */
void nivelNovato(Room* mansion) {
    simularExploracaoNovato(mansion);
}

void nivelAventureiro(Room* mansion) {
    printf("=== Nível Aventureiro: coletando pistas na mansão ===\n");
    BSTNode* bst = NULL;
    // criamos uma tabela hash vazia somente para compatibilidade (não usada aqui para associar)
    HashTable ht; hashInit(&ht);
    // array de suspeitos apenas para criar lista (não usada no Aventureiro)
    const char* suspectsNames[] = {"Joaquim","Mariana","Carlos","Fernanda"};
    Suspect* suspects = createSuspectList(4, suspectsNames);

    coletarPistasEAssociar(mansion, &bst, &ht, suspects, 4, mapaPistaParaSuspeito);

    printf("\nPistas coletadas (em ordem alfabética):\n");
    if (!bst) printf(" (Nenhuma pista encontrada)\n");
    else bstInOrder(bst);

    // liberar
    bstFree(bst);
    hashFree(&ht);
    freeSuspectList(suspects, 4);
    printf("=== Fim do Nível Aventureiro ===\n\n");
}

void nivelMestre(Room* mansion) {
    printf("=== Nível Mestre: associando pistas a suspeitos (hash) ===\n");
    BSTNode* bst = NULL;
    HashTable ht; hashInit(&ht);

    const char* suspectsNames[] = {"Joaquim","Mariana","Carlos","Fernanda"};
    int suspectCount = 4;
    Suspect* suspects = createSuspectList(suspectCount, suspectsNames);

    coletarPistasEAssociar(mansion, &bst, &ht, suspects, suspectCount, mapaPistaParaSuspeito);

    printf("\nPistas coletadas (em ordem):\n");
    if (!bst) printf(" (Nenhuma pista encontrada)\n");
    else bstInOrder(bst);

    printf("\n");
    hashPrintAll(&ht);

    imprimirSuspeitoMaisCitado(suspects, suspectCount);

    // liberar
    bstFree(bst);
    hashFree(&ht);
    freeSuspectList(suspects, suspectCount);
    printf("=== Fim do Nível Mestre ===\n\n");
}

/* ----------------------------- Programa principal com menu ----------------------------- */
int main() {
    Room* mansion = construirMansao();

    printf("=== Detective Quest (3 níveis) - Demonstração com conteúdos pré-definidos ===\n");
    printf("Salas, pistas e suspeitos estão pré-definidos no código.\n");
    printf("Escolha uma opção:\n");
    printf("  1 - Novato (árvore binária: navegação simulada)\n");
    printf("  2 - Aventureiro (BST de pistas)\n");
    printf("  3 - Mestre (BST + tabela hash de pistas->suspeitos)\n");
    printf("  0 - Executar os 3 níveis em sequência (demonstração completa)\n");
    printf("Escolha: ");

    int choice = -1;
    if (scanf("%d", &choice) != 1) {
        printf("Entrada inválida. Executando demonstração completa.\n");
        choice = 0;
    }

    printf("\n");

    if (choice == 1) nivelNovato(mansion);
    else if (choice == 2) nivelAventureiro(mansion);
    else if (choice == 3) nivelMestre(mansion);
    else if (choice == 0) {
        nivelNovato(mansion);
        nivelAventureiro(mansion);
        nivelMestre(mansion);
    } else {
        printf("Opção inválida. Encerrando.\n");
    }

    liberarSalas(mansion);
    return 0;
}
