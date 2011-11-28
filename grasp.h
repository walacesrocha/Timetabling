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
} AuxGrasp;


void ordenaDisiciplinasPorDificuldade(Problema *p, AuxGrasp *auxGrasp);

AuxGrasp *geraSolucaoInicialGrasp(Problema *p);

int getTotalHorariosViaveis(Problema *p, AuxGrasp* auxGrasp, int aula);

Individuo *buscaLocalGrasp(Problema*p, Individuo *indInicial);

Individuo *grasp(Problema*p);
