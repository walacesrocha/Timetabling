#ifndef __INDIVIDUO_h
#define __INDIVIDUO_h

#include "problema.h"
#include "agenda.h"
#include <stdio.h>
#include <time.h>

#define MAXRAND 101
#define MAXTENTATIVAS 50

typedef struct individuo{
	int *aula;	//vetor de aulas
	float fitness;
	int n;	//tamanho do individuo
}Individuo;


int numero_aleatorio (int maxrand);
void troca_par (Individuo *a, int pos1, int pos2);

int troca_par_restrita (Problema *p, Individuo *indiv, int pos1, int pos2);

Individuo *alocaIndividuo (void);

////////
void criaIndividuo (Individuo *a, Problema *p);

/* cria um Individuo inicialmente ordenado e em seguida realizando trocas entre 2 posicoes do mesmo nTrocas vezes*/
Individuo *geraIndividuoAleatorio2 (Problema *p, int nTrocas);

float violacoesHard1 (Problema *p, Individuo *a, int pos);

float violacoesHard3(Problema *p, Individuo *a, int pos);

float violacoesHard4(Problema *p, Individuo *a, int pos);

float somaViolacoesHardLocal(Problema *p, Individuo *a, int pos);

Individuo *geraIndividuoAleatorio(Problema *p, int nTrocas);

/* preenche o vetor com as posicoes*/
void inicializaIndividuo (Individuo *a, int numero_total_aulas);

void troca_par(Individuo *a, int pos1, int pos2);


void embaralhaIndividuo (Individuo *a, int nTrocas);

void ordenaIndividuo (Individuo *ind);

void liberaIndividuo (Individuo *a);

int ehAula(Problema *p, int valor);

/////////////
void preenche_individuo_teste (Individuo *a, int tam);


void imprimeIndividuo (Individuo *a);

/*imprime com espacos separando dias e salta linha a cada sala*/
void imprimeIndividuo2 (Problema *p, Individuo *a);

void imprimeIndividuo3 (Problema *p, Individuo *a);

void getDiaPeriodo (Problema *p, int pos, int *dia, int *periodo);

int getPeriodo (Problema *p, int pos);

int getDia (Problema *p, int pos);

int salaDiaHorarioParaPos (Problema *p, int sala, int dia, int horario);


#endif
