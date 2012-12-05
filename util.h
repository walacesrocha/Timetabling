/* 
 * File:   util.h
 * Author: walace
 *
 * Created on 10 de Maio de 2011, 13:54
 */

#ifndef _UTIL_H
#define	_UTIL_H

#include "problema.h"
#include "individuo.h"


int getDiaFromPos(int pos, Problema *p);
int getPeriodoFromPos(int pos, Problema *p);
int getRoomFromPos(int pos, Problema *p);
float tempoDecorrido(clock_t t1);
void imprimeMatrizTimetable(Problema *p, Individuo *ind) ;
int getTimeslot(Problema *p, int pos);

int esgotouTempoLimite(Problema *p);

#endif	/* _UTIL_H */

