#include <stdio.h>
#include <stdlib.h>
#include "grasp.h"
#include "problema.h"

/**
 * Compara duas aulas em relação a dificuldade. 
 * O primeiro critério é indisponibilidade de horarios, seja por restrição inicial 
 * ou aulas do mesmo currículo/professor. Se empatar nesse critério verifica-se qual
 * disciplina está presente em mais currículo.
 * @param p
 * @param auxGrasp
 * @param a1
 * @param a2
 * @return 
 */
int comparaAulas(Problema *p, AuxGrasp *auxGrasp, int a1, int a2) {

    Disciplina *d1, *d2;

    d1 = acessaDisciplina(p, a1);
    d2 = acessaDisciplina(p, a2);

    if (strcmp(d1->nomeDisciplina, d2->nomeDisciplina) == 0) {
        //printf("Disciplinas iguais: %d %d\n", a1, a2);
        return 0;
    }

    if (d1->nIndisponibilidades < d2->nIndisponibilidades) {
        return -1;
    } else if (d1->nIndisponibilidades > d2->nIndisponibilidades) {
        return 1;
    } else {
        if (d1->nCurriculos < d2->nCurriculos) {
            return -1;
        } else if (d1->nCurriculos > d2->nCurriculos) {
            return 1;
        } else {
            return 0;
        }
    }
}

void ordenaDisiciplinasPorDificuldade(Problema *p, AuxGrasp *auxGrasp) {
    printf("ord GRASP\n");


    int i, j, chave;

    for (j = 1; j < auxGrasp->nCandidatos; j++) {
        chave = auxGrasp->candidatos[j];
        i = j - 1;
        while (i >= 0 && comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], chave) > 0) {
            auxGrasp->candidatos[i + 1] = auxGrasp->candidatos[i];
            i--;
        }
        auxGrasp->candidatos[i + 1] = chave;
    }

}

Individuo *geraTimetableVazio(Problema *p) {
    int i;
    Individuo *ind = alocaIndividuo();

    criaIndividuo(ind, p);

    for (i = 0; i < ind->n; i++) {
        ind->aula[i] = p->nAulas + 1; // marca todas as aulas como vazias
    }

    return ind;
}

AuxGrasp *geraAuxGrasp(Problema *p) {
    int i, j, k;
    AuxGrasp *auxGrasp;

    auxGrasp = (AuxGrasp*) malloc(sizeof (AuxGrasp));

    auxGrasp->candidatos = (int*) malloc(p->nAulas * sizeof (int));
    auxGrasp->explosao = (int*) malloc(p->nAulas * sizeof (int));
    auxGrasp->nCandidatos = p->nAulas;

    for (i = 0; i < p->nAulas; i++) {
        auxGrasp->explosao[i] = 0;
    }

    i = 0;
    for (j = 0; j < p->nDisciplinas; j++) {
        int nDisc = p->disciplinas[j].aulaInicial;
        for (k = 0; k < p->disciplinas[j].nAulas; k++) {
            auxGrasp->candidatos[i] = nDisc;
            nDisc++;
            i++;
        }
    }

    // cria a tabela-horario vazia 
    auxGrasp->ind = geraTimetableVazio(p);

    return auxGrasp;
}

int *getHorariosViaveis(Problema*p, AuxGrasp*auxGrasp, int aula) {
    int *horariosViaveis;
    int i, j, qtHorarios;
    Disciplina *disc;

    qtHorarios = p->nDias * p->nPerDias;

    horariosViaveis = (int*) malloc(qtHorarios * sizeof (int));

    // marca todos horarios inicialmente como viaveis
    for (i = 0; i < qtHorarios; i++) {
        horariosViaveis[i] = 1;
    }

    for (i = 0; i < qtHorarios; i++) {
        printf("%d ", horariosViaveis[i]);
    }
    printf("\n");
    // tira os horarios restritos
    disc = acessaDisciplina(p, aula);
    for (i = 0; i < disc->nIndisponibilidades; i++) {
        horariosViaveis[disc->indisponibilidades[i]] = 0;
    }

    for (i = 0; i < qtHorarios; i++) {
        printf("%d ", horariosViaveis[i]);
    }
    printf("\n");

    // verifica os conflitos curriculo/professor
    for (i = 0; i < qtHorarios; i++) {
        int temHorarioVago = 0;
        int temConflito = 0;

        // percorre todas as salas do horario
        for (j = 0; j < p->nSalas; j++) {

            int pos = i + qtHorarios*j;
            //printf("checking ... %d\n", pos);

            if (ehAula(p, auxGrasp->ind->aula[pos])) {// tem uma disciplina na sala
                Disciplina *discPos = acessaDisciplina(p, auxGrasp->ind->aula[pos]);

                // conflito de professor
                if (strcmp(disc->professor, discPos->professor) == 0) {
                    temConflito = 1;
                    break;

                    // conflito de curriculo
                } else if (aulasMesmoCurriculo(p, aula, auxGrasp->ind->aula[pos])) {
                    temConflito = 1;
                    break;
                }
            } else {
                temHorarioVago = 1; // tem pelo menos uma sala vaga no horario
            }
        }

        // se nao tem sala vaga ou algum conflito marca como "inviavel"
        if (!temHorarioVago || temConflito) {
            horariosViaveis[i] = 0;
        }
    }

    return horariosViaveis;
}

void alocaAula(Problema *p, AuxGrasp* auxGrasp, int aula) {
    int *horariosViaveis;
    int i, qtHorarios; // quantidade de horarios: dias x periodos

    qtHorarios = p->nDias * p->nPerDias;

    horariosViaveis = getHorariosViaveis(p, auxGrasp, aula);

    for (i = 0; i < qtHorarios; i++) {
        printf("%d ", horariosViaveis[i]);
    }
    printf("\n");

}

Individuo *geraSolucaoInicialGrasp(Problema *p) {
    int i, j, k;
    AuxGrasp *auxGrasp;

    auxGrasp = geraAuxGrasp(p);

    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    printf("\n");

    ordenaDisiciplinasPorDificuldade(p, auxGrasp);

    printf("==============================================================================\n");

    for (i = 0; i < auxGrasp->nCandidatos; i++) {
        printf("%d ", auxGrasp->candidatos[i]);
        //printf("%d\n", comparaAulas(p, auxGrasp, auxGrasp->candidatos[i], 5));
    }
    printf("\n");


    while (auxGrasp->nCandidatos > 0) {// enquanto ha candidatos a alocarF
        int aula;

        ordenaDisiciplinasPorDificuldade(p, auxGrasp);

        // aula "mais dificil"
        aula = auxGrasp->candidatos[auxGrasp->nCandidatos - 1];
        aula=12;
        printf("Escolheu aula %d\n", aula);

        auxGrasp->ind->aula[0] = 1;
        auxGrasp->ind->aula[20] = 2;
        //auxGrasp->ind->aula[40] = 3;
        alocaAula(p, auxGrasp, aula);

        break;
    }

    printf("Indisp.[%s]:\n", p->disciplinas[1].nomeDisciplina);
    for (i = 0; i < p->disciplinas[1].nIndisponibilidades; i++) {
        printf("%d\n", p->disciplinas[1].indisponibilidades[i]);
    }

    return auxGrasp->ind;
}
