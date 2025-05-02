#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

U8* ler_arquivo(const char* caminho, size_t* tamanho);
char* pegar_extensao_arquivo(const char* caminho);
char* setar_novo_nome(const char* nome_arquivo, const char* extensao_desejada);