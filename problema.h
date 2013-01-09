#ifndef __PROBLEMA_h
#define __PROBLEMA_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Estrutura que define um curriculo
 */
typedef struct curriculo {
    char nomeCurriculo[30];
    int nDisciplinas;
    char **disciplinas;
    int pVetor;
} Curriculo;

/*
 * Estrutura que define uma disciplina
 */
typedef struct disciplina {
    char nomeDisciplina[30];
    char professor[30];
    Curriculo **curriculos; //Leandro
    int nCurriculos; //Leandro

    /*cada indisponibilidade eh um timeslot da aula, ou seja coluna da aula*/
    int *indisponibilidades; //Leandro
    int nIndisponibilidades; //Leandro

    int *slotsDisponiveis; //Leandro
    int nSlotsDisponiveis; //Leandro

    int nAulas;
    int aulaInicial;
    int minDiasAula;
    int nAlunos;
    int pVetor;
} Disciplina;

/*
 * Estrutura que define uma sala
 */
typedef struct sala {
    char idSala[30];
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
    char instancia[40]; //Leandro
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
    long int tempoLimite;
    long int tempoMelhorResposta;

    float t0; //SA
    float tf; // rho = t0/tmin;
    float beta; // resfriamento
    int aceitaPioraSA;

    /* Parametros Grasp*/
    float txSwap;
    int buscaLocalGrasp;
    int maxIterGrasp;
    int nIterSemMelhoras;
    float threshold;
    int k; // tamanho da lista de vizinhos
    long nMoves, nSwaps;

    float mediaSolucoes;

    /*Leandro inicio*/
    int nDiasEperiodos; //dias * períodos
    char **matrizAulaSala; //matriz compatibilidade aula-sala (número de estudantes)
    char **matrizAulaAula; //matriz conflito aula-aula (aulas do mesmo currículo ou com mesmo professor)
    char **matrizAulaIndisponibilidades; //matriz com os dias e horarios disponiveis para cada aula

    int trocavazios; //testes remover  //////////////
    int trocaefetiva; //testes remover ///////////////
    /*Leandro fim*/

    float soft1, soft2, soft3, soft4;
    float f1, f2, f3;

    clock_t inicio, fim;

    float pAproveitamento;
    long trocaEfetiva,trocaNaoEfetiva;
    float pesoHard;
    float pesoRC,pesoMW,pesoIL,pesoRS;
    
    long vHard, semEfeito, comMelhora, comPiora;
    
    int seed;

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
