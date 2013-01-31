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
float somaViolacoesSoft2(Problema *p, Individuo *a, int marcaAulas);


float fitness(Problema *p, Individuo *i);

float fitnessHard(Problema *p, Individuo *i);

float fitnessSoft (Problema *p, Individuo *i);

int aulaIsolada (Problema *p, Individuo *a, int pos, int dia, int horario);


float somaViolacoesHardTroca (Problema *p, Individuo *a, int pos1, int pos2);

//elimina aulas repetidas na mesma coluna (aulas da mesma disciplina no mesmo horário e em salas diferentes)
//a funcao restauraAulasMultiplas retorna a tabela para encoding padrao e deve ser usada logo apos os procedimentos de tabela sem aulas multiplas.
//retorna o numero de eliminacoes, ou seja o numero de aulas que sao contadas como faltando
int eliminaAulasMultiplas (Problema *p, Individuo *ind);

//desfaz alteracao de eliminaAulasMultiplas
void restauraAulasMultiplas (Problema *p, Individuo *ind);


//elimina aulas repetidas na coluna cuja primeira posicao = ini
int eliminaAulasMultiplasLocal (Problema *p, Individuo *ind, int ini, int aula) ;

void restauraAulasMultiplasLocal (Problema *p, Individuo *ind, int ini);

float somaConflitosTeste(Problema *p, Individuo *a);

float somaIndisponibilidadesTeste(Problema *p, Individuo *a);

int aulaIsolada (Problema *p, Individuo *a, int pos, int dia, int horario);
int somaAulasIsoladas(Problema *p, Individuo *ind) ;

#endif
