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



void buscaLocalHard(Problema *p, Individuo *ind);
void buscaLocalSoft(Problema *p, Individuo *ind);
Individuo* buscaLocalSoft2(Problema *p, Individuo *ind, int fase);
Individuo *buscaLocal2Etapas(Problema *p, Individuo *ind, int fase);
Individuo *buscaLocal(Problema *p, Individuo *ind, int fase);




#endif	/* _BUSCALOCAL_H */

