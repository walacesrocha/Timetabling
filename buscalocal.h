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

typedef enum {
    MOVE,
    SWAP,
    TIME_MOVE,
    ROOM_MOVE,
    ROOMS,
    COMPACT,
    MIN_WORKING_DAYS_MOVE,
    KEMPE
} Movimento;

typedef struct neighbour {
    int p1;
    int p2;
    float deltaHard;
    float deltaSoft;
    Movimento m;
} Neighbour;



void buscaLocalHard(Problema *p, Individuo *ind);
void buscaLocalSoft(Problema *p, Individuo *ind);
Individuo* buscaLocalSoft2(Problema *p, Individuo *ind, int fase);
Individuo *buscaLocal2Etapas(Problema *p, Individuo *ind, int fase);
Individuo *buscaLocal(Problema *p, Individuo *ind, int fase);
ListaVizinhos *alocaListaVizinhos(int k);
void desalocaListaVizinhos(ListaVizinhos *listaVizinhos);
void geraListaVizinhos(Problema *p, Individuo*ind, ListaVizinhos *listaVizinhos);

Individuo *geraVizinho3(Problema *p, Individuo *ind);

Individuo *buscaLocalGraspVNS(Problema*p, Individuo *indInicial);

Individuo *buscaLocalTimeslot(Problema *p, Individuo *ind);

Individuo *move(Problema *p, Individuo *ind);
Individuo *swap(Problema *p, Individuo *ind);
Individuo *lectureMove(Problema *p, Individuo *ind);
Individuo *timeMove(Problema *p, Individuo *ind);
Individuo *roomMove(Problema *p, Individuo *ind);
Individuo *rooms(Problema *p, Individuo *ind);
Individuo *compact(Problema *p, Individuo *ind);
Individuo *kempeSwap(Problema *p, Individuo * ind);

Neighbour *geraSwap(Problema *p, Individuo *ind);
Neighbour *geraMove(Problema *p, Individuo *ind);
Neighbour *geraTimeMove(Problema *p, Individuo *ind);
Neighbour *geraRoomMove(Problema *p, Individuo *ind);
Neighbour *geraMinWorkingDaysMove(Problema *p, Individuo *ind);
Neighbour *geraIsolatedLectureMove(Problema *p, Individuo *ind);



#endif	/* _BUSCALOCAL_H */

