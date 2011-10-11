#ifndef __IO_h
#define __IO_h


#include "problema.h"
#include "populacao.h"

Problema *lerInstancia(char *nomeArquivo);
void imprimeInstancia(Problema *p);
void imprimeResposta(Problema *p, Populacao *populacao);

#endif
