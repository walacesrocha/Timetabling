#include "individuo.h"

typedef struct auxGrasp {
    int *candidatos;
    int nCandidatos;
    int *explosao;
} AuxGrasp;

void ordenaDisiciplinas(AuxGrasp *p);

Individuo *geraSolucaoInicialGrasp(Problema *p);
