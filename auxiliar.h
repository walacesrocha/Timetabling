#ifndef __AUXILIAR_h
#define __AUXILIAR_h

#include "problema.h"
#include "fitness.h"

//Matriz Aula-Sala
void alocaMatrizAulaSala (Problema *p);

void imprimeMatrizAulaSala (Problema *p);

void preencheMatrizAulaSala (Problema *p);
void liberaMatrizAulaSala (Problema *p);

//Matriz Aula-Aula
void alocaMatrizAulaAula (Problema *p);
void imprimeMatrizAulaAula (Problema *p);
void preencheMatrizAulaAula (Problema *p);


int aulasConflitantes (Problema *p, int a1, int a2);

int aulasMesmaDisciplina3 (Problema *p, int a1, int a2);
void liberaMatrizAulaAula (Problema *p);

//Matriz Aula-Indisponibilidades
void preencheMatrizAulaIndisponibilidades (Problema *p);
void imprimeMatrizAulaIndisponibilidades (Problema *p);
int horarioIndisponivel(Problema *p, int aula, int pos);
void liberaMatrizAulaIndisponibilidades (Problema *p);

//Slots Disponiveis
void preencheSlotsDisponiveisTodasDisciplinas (Problema *p);
void imprimeSlotsDisponiveisTodasDisciplinas (Problema *p);

void liberaSlotsDisponiveisTodasDisciplinas (Problema *p);
    
#endif
