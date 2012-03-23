#include "individuo.h"
#include "problema.h"

typedef struct alocacaoAula {
    int nrAula;
    int horario;
    int sala;
    int custo;
    int id;
} AlocacaoAula;

typedef struct auxGrasp {
    Individuo *ind; // tabela horario que será construída pelo grasp
    int *candidatos;
    int nCandidatos;
    int *explosao;
    AlocacaoAula **vetorPossibilidades;
    int nrPossibilidades;

    Individuo **poolElite; // pool de solucoes elite
    int tPool; // tamanho do pool
    int nElites; // quantidade de solucoes elite no pool
    float piorSolucao;
    int posicaoPior;
} AuxGrasp;


void ordenaDisiciplinasPorDificuldade(Problema *p, AuxGrasp *auxGrasp);

void geraSolucaoInicialGrasp(Problema *p, AuxGrasp*auxGrasp);

int getTotalHorariosViaveis(Problema *p, AuxGrasp* auxGrasp, int aula);

Individuo *buscaLocalGraspProfundidade(Problema*p, Individuo *indInicial);

Individuo *grasp(Problema*p);
