#ifndef __FITNESS_h
#define __FITNESS_h

#include "problema.h"
#include "agenda.h"
#include "individuo.h"
#include "auxiliar.h"
#include <stdio.h>
#include <time.h>


float somaViolacoesHard (Problema *p, Individuo *a);

float somaViolacoesSoft (Problema *p, Individuo *a);

float fitness(Problema *p, Individuo *i);

float fitnessHard(Problema *p, Individuo *i);

int aulaIsolada (Problema *p, Individuo *a, int pos, int dia, int horario);


float somaViolacoesHardTroca (Problema *p, Individuo *a, int pos1, int pos2);


#endif
