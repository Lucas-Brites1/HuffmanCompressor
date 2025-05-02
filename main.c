#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "files.h"

void debug_decodificar(Codigo* codigo, no_arvore* raiz);
void limpeza_completa(U8* dados, Tabela_de_frequencias* tab,
                     lista_t* lista, no_arvore* arvore,
                     Codigo** dicionario, Codigo* codigo,
                     char* decodificado);
                     
void argumentos_terminal(char** args, char** nome_arquivo_original, char** nome_arquivo_saida);

int main(int argc, char* argv[]) {
    U8* dados_original = NULL;
    Tabela_de_frequencias* tabela = NULL;
    lista_t* lista = NULL;
    no_arvore* arvore = NULL;
    Codigo** dicionario = NULL;
    Codigo codigo_atual = {0};
    char* decodificado = NULL;
    char* nome_arquivo = NULL;
    char* nome_saida = NULL;
    
    argumentos_terminal(argv, &nome_arquivo, &nome_saida);
    char* extensao_arquivo = pegar_extensao_arquivo(nome_arquivo);
    const char* nome_arquivo_original =  nome_arquivo;
    const char* nome_arquivo_codificado = setar_novo_nome(nome_arquivo, "huf");
    const char* nome_arquivo_decodificado = setar_novo_nome(nome_saida, extensao_arquivo);

    // Ler o arquivo passado:
    size_t tamanho_original;
    dados_original = ler_arquivo(nome_arquivo_original, &tamanho_original);
    if (!dados_original || tamanho_original == 0) {
        fprintf(stderr, "Erro: Falha ao ler arquivo original '%s' ou arquivo vazio\n", nome_arquivo_original);
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        return EXIT_FAILURE;
    }

    printf("Arquivo original '%s' lido com sucesso. Tamanho: %zu bytes\n", nome_arquivo_original, tamanho_original);

    // Analisando a frequencia dos bytes:
    tabela = nova_tabela_de_frequencias();
    if (!tabela) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < tamanho_original; i++) {
        if (!inclua_byte(dados_original[i], tabela)) {
            fprintf(stderr, "Erro: Falha ao incluir byte 0x%02X na tabela\n", dados_original[i]);
            limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
            return EXIT_FAILURE;
        }
    }

    printf("\n=== Tabela de Frequências ===\n");
    print_tabela_frequencias(tabela);

    // Construindo a lista encadeada por meio da tabela de frequencias
    printf("\n=== Lista Encadeada ===\n");
    lista = criar_lista_encadeada(tabela);
    if (!lista) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        return EXIT_FAILURE;
    }
    print_lista(lista);

    // Construindo a arvore
    printf("\n=== Árvore de Huffman ===\n");
    arvore = criar_arvore(lista);
    if (!arvore) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        return EXIT_FAILURE;
    }
    print_arvore(arvore, 0);

    printf("\n=== Verificação da Árvore ===\n");
    verificar_arvore(arvore);

    // Gerando o dicionario que mapeia os codigos para os caminhos das folhas que representam os bytes
    dicionario = alocar_dicionario();
    if (!dicionario) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        return EXIT_FAILURE;
    }

    if (!novo_codigo(&codigo_atual)) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        return EXIT_FAILURE;
    }

    gerar_dicionario(dicionario, arvore, codigo_atual);

    printf("\n=== Dicionário de Códigos ===\n");
    verificar_dicionario(dicionario);

    // Codificando e decodificndo
    printf("\n=== Codificação e Escrita no Arquivo '%s' ===\n", nome_arquivo_codificado);
    FILE* arquivo_para_codificar = fopen(nome_arquivo_original, "rb");
    FILE* arquivo_codificado_ptr = fopen(nome_arquivo_codificado, "wb");
    if (!arquivo_para_codificar || !arquivo_codificado_ptr) {
        perror("Erro ao abrir arquivos para codificação");
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        if (arquivo_para_codificar) fclose(arquivo_para_codificar);
        if (arquivo_codificado_ptr) fclose(arquivo_codificado_ptr);
        return EXIT_FAILURE;
    }

    codificar(arquivo_para_codificar, arquivo_codificado_ptr, dicionario);

    fclose(arquivo_para_codificar);
    fclose(arquivo_codificado_ptr);

    printf("Arquivo '%s' codificado com sucesso.\n", nome_arquivo_codificado);

    printf("\n=== Decodificação do Arquivo '%s' para '%s' ===\n",
           nome_arquivo_codificado, nome_arquivo_decodificado);

    FILE* arquivo_para_decodificar = fopen(nome_arquivo_codificado, "rb");
    FILE* arquivo_decodificado_ptr = fopen(nome_arquivo_decodificado, "wb");
    if (!arquivo_para_decodificar || !arquivo_decodificado_ptr) {
        perror("Erro ao abrir arquivos para decodificação");
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, decodificado);
        if (arquivo_para_decodificar) fclose(arquivo_para_decodificar);
        if (arquivo_decodificado_ptr) fclose(arquivo_decodificado_ptr);
        return EXIT_FAILURE;
    }

    decodificar(arquivo_para_decodificar, arquivo_decodificado_ptr);

    fclose(arquivo_para_decodificar);
    fclose(arquivo_decodificado_ptr);

    printf("Arquivo '%s' decodificado com sucesso para '%s'.\n",
           nome_arquivo_codificado, nome_arquivo_decodificado);

    printf("\n=== Verificação de Integridade ===\n");
    U8* dados_decodificados = NULL;
    size_t tamanho_decodificado;
    dados_decodificados = ler_arquivo(nome_arquivo_decodificado, &tamanho_decodificado);
    if (!dados_decodificados) {
        fprintf(stderr, "Erro ao ler arquivo decodificado '%s'\n", nome_arquivo_decodificado);
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, NULL);
        return EXIT_FAILURE;
    }

    if (tamanho_original == tamanho_decodificado &&
        memcmp(dados_original, dados_decodificados, tamanho_original) == 0) {
        printf("Sucesso! Arquivo decodificado é idêntico ao original.\n");
    } else {
        fprintf(stderr, "ERRO: Arquivo decodificado é diferente do original!\n");
        printf("Tamanho original: %zu, Tamanho decodificado: %zu\n",
               tamanho_original, tamanho_decodificado);

        for (size_t i = 0; i < (tamanho_original > tamanho_decodificado ? tamanho_decodificado : tamanho_original); i++) {
            if (dados_original[i] != dados_decodificados[i]) {
                printf("Primeira diferença na posição %zu: Original 0x%02X, Decodificado 0x%02X\n",
                       i, dados_original[i], dados_decodificados[i]);
                break;
            }
        }
    }

    limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL, (char*)dados_decodificados); 
    free_codigo(&codigo_atual); 

    return EXIT_SUCCESS;
}

void debug_decodificar(Codigo* codigo, no_arvore* raiz) {
    if (!codigo || !raiz) return;

    no_arvore* atual = raiz;
    printf("\nDebug detalhado da decodificação (primeiros 20 bits):\n");

    for (size_t i = 0; i < codigo->tamanho && i < 20; i++) {
        U8 byte_idx = i / 8;
        U8 bit_offset = i % 8;
        U8 bit = (codigo->byte[byte_idx] >> (7 - bit_offset)) & 1;

        printf("Bit %zu: %d -> ", i, bit);
        atual = bit ? atual->direita : atual->esquerda;

        if (!atual) {
            printf("ERRO: Nó nulo encontrado!\n");
            return;
        }

        if (eh_folha(atual)) {
            printf("Decodificado: '%c' (0x%02X)\n",
                   isprint(atual->informacao.byte) ? atual->informacao.byte : '.',
                   atual->informacao.byte);
            atual = raiz;
        } else {
            printf("Nó interno\n");
        }
    }
}

void limpeza_completa(U8* dados, Tabela_de_frequencias* tab,
                     lista_t* lista, no_arvore* arvore,
                     Codigo** dicionario, Codigo* codigo,
                     char* decodificado) {
    printf("[DEBUG] Iniciando limpeza completa...\n");
    if (decodificado) {
        printf("[DEBUG] Liberando decodificado: %p\n", decodificado);
        free(decodificado);
    }
    if (codigo) {
        printf("[DEBUG] Liberando codigo_gerado...\n");
        free_codigo(codigo);
    }
    if (dicionario) {
        printf("[DEBUG] Liberando dicionario: %p\n", dicionario);
        liberar_dicionario(dicionario);
    }
    if (arvore) {
        printf("[DEBUG] Liberando arvore: %p\n", arvore);
        liberar_arvore(arvore);
    }
    if (lista) {
        printf("[DEBUG] Liberando lista: %p\n", lista);
        free(lista);
    }
    if (tab) {
        printf("[DEBUG] Liberando tabela: %p\n", tab);
        free(tab);
    }
    if (dados) {
        printf("[DEBUG] Liberando dados: %p\n", dados);
        free(dados);
    }
    printf("[DEBUG] Limpeza completa finalizada.\n");
}

void argumentos_terminal(char** args, char** nome_arquivo_original, char** nome_arquivo_saida) {
    if(args[1]) {
        *nome_arquivo_original = args[1];
        *nome_arquivo_saida = args[2];
        if(args[2] == NULL) {
            *nome_arquivo_saida = *nome_arquivo_original;
        }
    }
}   