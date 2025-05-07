#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "files.h"

int main(int argc, char* argv[]) {
    U8* dados_original = NULL;
    Tabela_de_frequencias* tabela = NULL;
    lista_t* lista = NULL;
    no_arvore* arvore = NULL;
    Codigo** dicionario = NULL;
    Codigo codigo_atual = {0};
    char* nome_arquivo = NULL;
    char* nome_saida = NULL;

    char* instrucao = argumentos_terminal(argc, argv, &nome_arquivo, &nome_saida);
    if (strcmp(instrucao, "ERRO") == 0) {
        printf("\nParâmetros inválidos.\nUse -help ou -h para ajuda (./huffman -help)\n");
        exit(EXIT_FAILURE);
    }

    char* extensao_arquivo = pegar_extensao_arquivo(nome_arquivo);
    const char* nome_arquivo_original = nome_arquivo;
    const char* nome_arquivo_codificado = setar_novo_nome(nome_arquivo, "huf");
    const char* nome_arquivo_decodificado = argv[3];

    // Ler arquivo original
    size_t tamanho_original;
    dados_original = ler_arquivo(nome_arquivo_original, &tamanho_original);
    if (!dados_original || tamanho_original == 0) {
        fprintf(stderr, "Erro: Falha ao ler arquivo original '%s' ou arquivo vazio.\n", nome_arquivo_original);
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
        return EXIT_FAILURE;
    }
    printf("Arquivo original '%s' lido com sucesso. Tamanho: %zu bytes\n", nome_arquivo_original, tamanho_original);

    // Tabela de frequências
    tabela = nova_tabela_de_frequencias();
    if (!tabela) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < tamanho_original; i++) {
        if (!inclua_byte(dados_original[i], tabela)) {
            fprintf(stderr, "Erro: Falha ao incluir byte 0x%02X na tabela\n", dados_original[i]);
            limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
            return EXIT_FAILURE;
        }
    }

    // Lista encadeada
    printf("\n=== Lista Encadeada ===\n");
    lista = criar_lista_encadeada(tabela);
    if (!lista) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
        return EXIT_FAILURE;
    }

    // Árvore de Huffman
    printf("\n=== Árvore de Huffman ===\n");
    arvore = criar_arvore(lista);
    if (!arvore) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
        return EXIT_FAILURE;
    }

    // Dicionário de códigos
    dicionario = alocar_dicionario();
    if (!dicionario) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
        return EXIT_FAILURE;
    }

    if (!novo_codigo(&codigo_atual)) {
        limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
        return EXIT_FAILURE;
    }

    gerar_dicionario(dicionario, arvore, codigo_atual);

    // Codificação
    if (strcmp(instrucao, "-c") == 0) {
        printf("\n=== Codificação: '%s' => '%s' ===\n", nome_arquivo_original, nome_arquivo_codificado);
        FILE* in = fopen(nome_arquivo_original, "rb");
        FILE* out = fopen(nome_arquivo_codificado, "wb");
        if (!in || !out) {
            perror("Erro ao abrir arquivos para codificação");
            if (in) fclose(in);
            if (out) fclose(out);
            limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
            return EXIT_FAILURE;
        }

        codificar(in, out, dicionario);
        fclose(in);
        fclose(out);

        printf("Codificação concluída com sucesso: %s\n", nome_arquivo_codificado);
    }

    // Decodificação
    if (strcmp(instrucao, "-d") == 0) {
        printf("\n=== Decodificação: '%s' => '%s' ===\n", nome_arquivo_codificado, nome_arquivo_decodificado);
        FILE* in = fopen(nome_arquivo_codificado, "rb");
        FILE* out = fopen(nome_arquivo_decodificado, "wb");
        if (!in || !out) {
            perror("Erro ao abrir arquivos para decodificação");
            if (in) fclose(in);
            if (out) fclose(out);
            limpeza_completa(dados_original, tabela, lista, arvore, dicionario, NULL);
            return EXIT_FAILURE;
        }

        decodificar(in, out);
        fclose(in);
        fclose(out);

        printf("Decodificação concluída com sucesso: %s\n", nome_arquivo_decodificado);
    }

    limpeza_completa(dados_original, tabela, lista, arvore, dicionario, &codigo_atual);
    return EXIT_SUCCESS;
}

// Limpa todos os dados alocados
void limpeza_completa(U8* dados, Tabela_de_frequencias* tab, lista_t* lista, no_arvore* arvore, Codigo** dicionario, Codigo* codigo) {
    printf("[DEBUG] Iniciando limpeza...\n");

    if (codigo) {
        free_codigo(codigo);
    }
    if (dicionario) {
        liberar_dicionario(dicionario);
    }
    if (arvore) {
        liberar_arvore(arvore);
    }
    if (lista) {
        free(lista);
    }
    if (tab) {
        free(tab);
    }
    if (dados) {
        free(dados);
    }

    printf("[DEBUG] Limpeza completa.\n");
}

// Processa argumentos do terminal
char* argumentos_terminal(int argc, char** argv, char** nome_arquivo_original, char** nome_arquivo_saida) {
    if (argc < 2) return "ERRO";

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printf("Uso: ./huffman [opção] <arquivo_entrada> [arquivo_saida]\n");
        printf("  -c           Codifica o arquivo\n");
        printf("  -d           Decodifica o arquivo\n");
        printf("  -h, --help   Exibe esta ajuda\n");
        exit(0);
    }

    if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "-d") == 0) {
        if (argc < 3) return "ERRO";
        *nome_arquivo_original = argv[2];
        *nome_arquivo_saida = (argc >= 4) ? argv[3] : argv[2];
        return argv[1]; // "-c" ou "-d"
    }

    return "ERRO";
}

//gcc arvore.c main.c codificar.c codigo.c decodificar.c dicionario.c files.c lista_encadeada.c tabela_frequencias.c -o huffman