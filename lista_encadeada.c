#include <stdio.h>
#include <stdlib.h>
#include "types.h"

//Lista
no_arvore* criar_no(elemento_t* elemento) {
    no_arvore* nodo = (no_arvore*)malloc(sizeof(no_arvore));
    if(nodo == NULL) {
        printf("Ocorreu um erro na alocacao de memoria. (criar_no)\n");
        return NULL; 
    }

    nodo->direita=NULL;
    nodo->esquerda=NULL;
    nodo->proximo=NULL;

    nodo->informacao = *(elemento);

    return nodo;
}

void inserir_ordenado(lista_t** lista, no_arvore* nodo) {
    no_arvore* novo_nodo = nodo; // novo Node que vai ser adicionado na lista
    no_arvore* atual = (*lista)->comeco; // aponta para o primerio elemento da lista
    no_arvore* anterior = NULL; // vai servir para fazermos troca de posicoes do atual com o novo_nodo sem perder a referencia do atual

    // Se a lista estiver vazia
    if (atual == NULL) {
        (*lista)->comeco = novo_nodo;
        novo_nodo->proximo = NULL;
        (*lista)->tamanho++;
        return;
    }

    // Encontra a posição correta para inserir o novo nó
    while (atual != NULL && novo_nodo->informacao.frequencia > atual->informacao.frequencia) {
        anterior = atual;
        atual = atual->proximo;
    }

    // Inserir no início
    if (anterior == NULL) {
        novo_nodo->proximo = (*lista)->comeco;
        (*lista)->comeco = novo_nodo;
    }
    // Inserir no meio ou no final
    else {
        novo_nodo->proximo = atual;
        anterior->proximo = novo_nodo;
    }

    (*lista)->tamanho++;
}

lista_t* criar_lista_encadeada(Tabela_de_frequencias* tabela) {
    lista_t* lista = (lista_t*)malloc(sizeof(lista_t));
    if(lista == NULL) {
        printf("Erro ao alocar memoria para lista. (criar_lista_encadeada).\n");
        return NULL;
    }

    lista->comeco=NULL;
    lista->tamanho=0;
    
    for(U16 i=0; i< 256; i++) {
        if(tabela->vetor[i] != NULL) {
            inserir_ordenado(&lista, tabela->vetor[i]);
        }
    }

    return lista;
}

void print_lista(lista_t* lista) {
    U16 i = 0;
    no_arvore* atual = lista->comeco;
    while(atual != NULL) {
        printf("[DEBUG] atual: %p\n", (void*)atual);

        printf("LISTA[%d] = (Byte: '%c' - Freq: %lu)\n", i, atual->informacao.byte, atual->informacao.frequencia);
        atual = atual->proximo;
        i++;
    }
    printf("\n");
}

no_arvore* remover_primeiro_no_lista(lista_t** lista) {
    if((*lista)->tamanho == 0) {
        printf("Lista nao possui elementos para serem removidos.");
        return NULL;
    }

    no_arvore* aux = (*lista)->comeco;
    (*lista)->comeco = aux->proximo; // 2 elemento se tornara o primeiro
    aux->proximo = NULL; 
    (*lista)->tamanho--;

    return aux;
}

