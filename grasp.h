#include "individuo.h"
#include "problema.h"

typedef struct auxGrasp {
    Individuo *ind; // tabela horario que será construída pelo grasp
    int *candidatos;
    int nCandidatos;
    int *explosao;
} AuxGrasp;

void ordenaDisiciplinasPorDificuldade(Problema *p, AuxGrasp *auxGrasp);

Individuo *geraSolucaoInicialGrasp(Problema *p);
