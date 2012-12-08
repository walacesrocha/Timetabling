/* 
 * File:   sa.h
 * Author: walace
 *
 * Created on 4 de Julho de 2011, 14:15
 */

#ifndef _SA_H
#define	_SA_H

#include "individuo.h"
#include "tabu.h"



Individuo *geraVizinho(Problema *p, Individuo *ind);
Individuo *geraVizinho2(Problema *p, Individuo *ind);
Individuo *geraVizinho2Tabu(Problema *p, Individuo *ind, Tabu *listaTabu);
Individuo *geraVizinho4(Problema *p, Individuo *ind);
Individuo *simulatedAnnealing(Problema*p, Individuo *indInicial);
Individuo *simulatedAnnealingVNS(Problema*p, Individuo *indInicial);
float funcaoObjetivo(Problema *p, Individuo *ind, float pesoHard);




#endif	/* _SA_H */

