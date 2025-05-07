#pragma once

// tipos inteiros
typedef char I8; // de -128 a 127
typedef unsigned char U8; // de 0 a 255 
typedef short int I16; // de -32768 a 32767
typedef unsigned short int U16; // de 0 a 65535
typedef int I32; // de -2147483648 a 2147483647
typedef unsigned int U32; // de 0 a 4294967295
typedef long int I64;
typedef unsigned long int U64;

// tipos reais
typedef float R32;
typedef double R64;
typedef long double R128;

// tipo boolean
typedef enum {
    FALSE,
    TRUE
} boolean;

// Struct pra cada byte do arquivo
typedef struct {
    U8 byte;
    U64 frequencia;
} elemento_t;

typedef struct Struct_do_no_de_arvore_binaria {
    struct Struct_do_no_de_arvore_binaria* esquerda;
    elemento_t informacao;
    struct Struct_do_no_de_arvore_binaria* direita;

    // ponteiro proximo vai auxiliar na montagem da lista encadeada e na insercao crescente (menor -> mainor)
    struct Struct_do_no_de_arvore_binaria* proximo;
} no_arvore;

typedef no_arvore* Ptr_de_no_de_arvore_binaria;

typedef struct {
    Ptr_de_no_de_arvore_binaria vetor [256];
    U16 quantidade_de_posicoes_preenchidas;
} Tabela_de_frequencias;

typedef struct {
    no_arvore* comeco;
    U16 tamanho;
} lista_t;

typedef struct {
    U8* byte;
    U8  capacidade; 
    U8  tamanho; 
} Codigo;

Tabela_de_frequencias* nova_tabela_de_frequencias ();
boolean inclua_byte (U8 byte, Tabela_de_frequencias* tab /* por referencia */);
void junte_nodos_no_inicio_do_vetor (Tabela_de_frequencias* tab /* por referencia */);

lista_t* criar_lista_encadeada(Tabela_de_frequencias* tabela);
void inserir_ordenado(lista_t** lista, no_arvore* nodo);
no_arvore* remover_primeiro_no_lista(lista_t** lista);
no_arvore* criar_no(elemento_t* elemento);

no_arvore* criar_arvore(lista_t* lista);
I8 eh_folha(no_arvore* raiz);
void verificar_arvore(no_arvore* raiz);

Codigo** alocar_dicionario();
void gerar_dicionario(Codigo** dicionario_alocado, no_arvore* raiz, Codigo codigo_atual);
void liberar_dicionario(Codigo** dicionario);

boolean novo_codigo (Codigo* c);
void free_codigo (Codigo* c);
boolean adiciona_bit (Codigo* c, U8 valor);
boolean joga_fora_bit (Codigo* c);
boolean clone (Codigo original, Codigo* copia);

void codificar(FILE* arquivo_entrada, FILE* arquivo_saida, Codigo** dicionario);
void decodificar(FILE* arquivo_codificado, FILE* arquivo_decodificado);

void liberar_arvore(no_arvore* raiz);
void liberar_dicionario(Codigo** dicionario);

void limpeza_completa(U8* dados, Tabela_de_frequencias* tab, lista_t* lista, no_arvore* arvore,Codigo** dicionario, Codigo* codigo);
char* argumentos_terminal(int argc, char** argv, char** nome_arquivo_original, char** nome_arquivo_saida);

