#ifndef __PROBLEMA_h
#define __PROBLEMA_h

#include <stdio.h>
#include <stdlib.h>

/*
 * Estrutura que define um curriculo
 */
typedef struct curriculo {
    char nomeCurriculo[20];
    int nDisciplinas;
    char **disciplinas;
} Curriculo;

/*
 * Estrutura que define uma disciplina
 */
typedef struct disciplina {
    char nomeDisciplina[30];
    char professor[20];
    Curriculo **curriculos; //Leandro
    int nCurriculos; //Leandro
    
    /*cada indisponibilidade eh um timeslot da aula, ou seja coluna da aula*/
    int *indisponibilidades; //Leandro
    int nIndisponibilidades; //Leandro
    int nAulas;
    int aulaInicial;
    int minDiasAula;
    int nAlunos;
} Disciplina;

/*
 * Estrutura que define uma sala
 */
typedef struct sala {
    char idSala[20];
    int capacidade;
} Sala;

/*
 * Estrutura que define uma restricao de indisponibilidade de horário
 */
typedef struct restricao {
    char nomeDisciplina[30];
    int dia;
    int periodo;
} Restricao;

/*
 * Estrutura que define uma instância de problema
 */
typedef struct problema {
    char nome[40];
    int nDisciplinas;
    int nSalas;
    int nDias;
    int nPerDias;
    int nCurriculos;
    int nRestricoes;
    int nAulas; //numero total de aulas
    int dimensao; // dimensao do problema: dias*periodos*salas
    Disciplina *disciplinas; // vetor de disciplinas
    Sala *salas; // vetor de salas
    Curriculo *curriculos; // vetor de curriculos
    Restricao *restricoes; // vetor de restricoes
    float txMutacao, txCrossover; // taxa percentual de mutação e crossover
    int maxTrocasMutacao;
    int tPopulacao; // tamanho da população
    float tempoLimite;
    float tempoMelhorResposta;
    
    float t0; //SA
    float rho; // rho = t0/tmin;
    float beta; // resfriamento
    
    /*Leandro inicio*/
    char **matrizAulaSala; //matriz compatibilidade aula-sala (número de estudantes)
    char **matrizAulaAula; //matriz conflito aula-aula (aulas do mesmo currículo ou com mesmo professor)
    /*Leandro fim*/
    
} Problema;

Problema *alocaProblema();
Disciplina *alocaDisciplinas(int nDisciplinas);
Sala *alocaSalas(int nSalas);
Curriculo *alocaCurriculos(int nCurriculos);
Restricao *alocaRestricoes(int nRestricoes);
char **alocaVetorDisciplinas(int nDisciplinas);
int aulasMesmaDisciplina(Problema *p, int a1, int a2);
int disciplinasMesmoCurriculo(Problema *p, char *d1, char*d2);
int aulasMesmoCurriculo(Problema *p, int a1, int a2);

/* Recupera o nome da disciplina pelo numero da aula */
char *getDisciplina(Problema *p, int aula);

/* Liberação de memória */
void desalocaProblema(Problema *p);
void desalocaDisciplina(Disciplina *d);
void desalocaCurriculo(Curriculo *c);
void desalocaSala(Sala *s);
void desalocaRestricao(Restricao *r);

/*Leandro inicio*/
void calculaNumeroTotalAulas(Problema *p);
Disciplina *acessaDisciplina(Problema *p, int aula);
void insereCurriculosNasDisciplinas(Problema *p);
void imprimeTodasDisciplinas(Problema *p);
void insereIndisponibilidadesNasDisciplinas(Problema *p);
void ordenaDisciplinasPorRestricoes(Problema *p);
int getTimeSlotFromPos(int pos, Problema *p);

//refazendo funcoes para novas estruturas
int disciplinasMesmoCurriculo2(Problema *p, Disciplina *d1, Disciplina *d2);

int aulasMesmoCurriculo2(Problema *p, int a1, int a2);

int aulasMesmoProfessor(Problema *p, int aula1, int aula2);

/*Leandro fim*/

#endif
