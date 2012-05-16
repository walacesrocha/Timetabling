/* 
 * File:   sa.h
 * Author: walace
 *
 * Created on 4 de Julho de 2011, 14:15
 */

#ifndef _SA_H
#define	_SA_H

#include "individuo.h"



Individuo *geraVizinho(Problema *p, Individuo *ind);
Individuo *geraVizinho2(Problema *p, Individuo *ind);
Individuo *geraVizinho3(Problema *p, Individuo *ind);
Individuo *simulatedAnnealing(Problema*p, Individuo *indInicial);
float funcaoObjetivo(Problema *p, Individuo *ind);




#endif	/* _SA_H */

