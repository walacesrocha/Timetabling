/* 
 * File:   buscalocal.h
 * Author: walace
 *
 * Created on 7 de Junho de 2011, 14:40
 */

#ifndef _BUSCALOCAL_H
#define	_BUSCALOCAL_H

#include "problema.h"
#include "individuo.h"

typedef struct listaVizinhos {
    int *pos1;
    int *pos2;
} ListaVizinhos;



void buscaLocalHard(Problema *p, Individuo *ind);
void buscaLocalSoft(Problema *p, Individuo *ind);
Individuo* buscaLocalSoft2(Problema *p, Individuo *ind, int fase);
Individuo *buscaLocal2Etapas(Problema *p, Individuo *ind, int fase);
Individuo *buscaLocal(Problema *p, Individuo *ind, int fase);
ListaVizinhos *alocaListaVizinhos(int k);
void desalocaListaVizinhos(ListaVizinhos *listaVizinhos);
void geraListaVizinhos(Problema *p,Individuo*ind, ListaVizinhos *listaVizinhos);

Individuo *geraVizinho3(Problema *p, Individuo *ind);


#endif	/* _BUSCALOCAL_H */

