#ifndef __AGENDA_h
#define __AGENDA_h

#include "problema.h"
#define HORARIOVAZIO -1

typedef struct agenda{
    Disciplina *disp;

    int **deh; //matriz dia e horario: cada linha (0 a nDias-1) representa um dia e cada coluna representa um horario

    int *salas;
    int nSalas;
}Agenda;


Agenda *criaAgenda (Problema *p, Disciplina *d);

void liberaAgenda (Problema *p, Agenda *a);

void inicializaAgenda (Problema *p, Agenda *a);

int nRestricoesMinDias (Problema *p, Agenda *a);

void imprimeAgenda (Problema *p, Agenda *a);

int setAgenda(Agenda *a, int aula, int sala, int dia, int horario);

#endif
