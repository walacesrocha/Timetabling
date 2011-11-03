#include <stdio.h>
#include "io.h"
#include "populacao.h"
#include "util.h"

/*
 * Le um arquivo .ctt e armazena os dados na estrutura Problema
 */
Problema *lerInstancia(char *nomeArquivo) {
    Problema *p;
    FILE *f;
    int i, j;
    int nrAula;

    // abre arquivo da instancia para leitura dos dados
    f = fopen(nomeArquivo, "r");
    if (f == NULL) {
        printf("Erro de abertura do arquivo %s\n", nomeArquivo);
        exit(1);
    }

    p = alocaProblema();

    // dados básicos do cabeçalho
    fscanf(f, "Name: %s\n", p->nome);
    fscanf(f, "Courses: %d\n", &(p->nDisciplinas));
    fscanf(f, "Rooms: %d\n", &(p->nSalas));
    fscanf(f, "Days: %d\n", &(p->nDias));
    fscanf(f, "Periods_per_day: %d\n", &(p->nPerDias));
    fscanf(f, "Curricula: %d\n", &(p->nCurriculos));
    fscanf(f, "Constraints: %d\n", &(p->nRestricoes));

    //printf("Leu cabeçalho\n");

    // dados das disciplinas
    fscanf(f, "\nCOURSES:\n");

    p->disciplinas = alocaDisciplinas(p->nDisciplinas);

    nrAula = 0; // contador de aulas
    for (i = 0; i < p->nDisciplinas; i++) {
        fscanf(f, "%s %s %d %d %d\n", (p->disciplinas + i)->nomeDisciplina,
                (p->disciplinas + i)->professor,
                &((p->disciplinas + i)->nAulas),
                &((p->disciplinas + i)->minDiasAula),
                &((p->disciplinas + i)->nAlunos));

        // registra o numero da primeira aula da disciplina
        (p->disciplinas + i)->aulaInicial = nrAula + 1;

        // incrementa o contador de aulas
        nrAula += (p->disciplinas + i)->nAulas;
    }

    // armazena o total de aulas no problema
    p->nAulas = nrAula;

    // dimensao do problema
    p->dimensao = p->nDias * p->nPerDias * p->nSalas;

    //printf("Leu disciplinas\n");

    // dados das salas
    fscanf(f, "\nROOMS:\n");

    p->salas = alocaSalas(p->nSalas);

    for (i = 0; i < p->nSalas; i++) {
        fscanf(f, "%s %d\n", (p->salas + i)->idSala,
                &((p->salas + i)->capacidade));
    }

    //printf("Leu salas\n");

    // dados dos curriculos
    fscanf(f, "\nCURRICULA:\n");

    p->curriculos = alocaCurriculos(p->nCurriculos);

    for (i = 0; i < p->nCurriculos; i++) {
        fscanf(f, "%s %d", (p->curriculos + i)->nomeCurriculo,
                &((p->curriculos + i)->nDisciplinas));

        (p->curriculos + i)->disciplinas = alocaVetorDisciplinas((p->curriculos + i)->nDisciplinas);

        for (j = 0; j < (p->curriculos + i)->nDisciplinas; j++) {
            fscanf(f, "%s ", (p->curriculos + i)->disciplinas[j]);
        }
    }

    //printf("Leu curriculos\n");

    // dados das restricoes
    fscanf(f, "\nUNAVAILABILITY_CONSTRAINTS:\n");

    p->restricoes = alocaRestricoes(p->nRestricoes);

    for (i = 0; i < p->nRestricoes; i++) {
        fscanf(f, "%s %d %d\n", (p->restricoes + i)->nomeDisciplina,
                &((p->restricoes + i)->dia), &((p->restricoes + i)->periodo));
    }

    //printf("Leu restricoes\n");

    insereCurriculosNasDisciplinas(p);
    insereIndisponibilidadesNasDisciplinas(p);
    ordenaDisciplinasPorRestricoes(p);
    preencheMatrizAulaSala(p);
    preencheMatrizAulaAula(p);

    return p;
}

/*
 * Imprime os dados armazenados na estrutura problema
 */
void imprimeInstancia(Problema *p) {
    int i, j;

    printf("Instância: %s\n", p->nome);
    printf("Disciplinas: %d\n", p->nDisciplinas);
    printf("Salas: %d\n", p->nSalas);
    printf("Dias: %d\n", p->nDias);
    printf("Periodos por dia: %d\n", p->nPerDias);
    printf("Curriculos: %d\n", p->nCurriculos);
    printf("Restricoes: %d\n\n", p->nRestricoes);

    printf("Disciplinas:\n");
    for (i = 0; i < p->nDisciplinas; i++) {
        printf("%s %s %d[%d] %d %d\n", (p->disciplinas + i)->nomeDisciplina,
                (p->disciplinas + i)->professor,
                (p->disciplinas + i)->nAulas,
                (p->disciplinas + i)->aulaInicial,
                (p->disciplinas + i)->minDiasAula,
                (p->disciplinas + i)->nAlunos);
    }

    printf("Salas:\n");
    for (i = 0; i < p->nSalas; i++) {
        printf("%s %d\n", (p->salas + i)->idSala, (p->salas + i)->capacidade);
    }

    printf("Curriculos:\n");
    for (i = 0; i < p->nCurriculos; i++) {
        printf("%s %d ", (p->curriculos + i)->nomeCurriculo, (p->curriculos + i)->nDisciplinas);

        for (j = 0; j < (p->curriculos + i)->nDisciplinas; j++) {
            printf("%s ", (p->curriculos + i)->disciplinas[j]);
        }
        printf("\n");
    }

    printf("Restricoes:\n");
    for (i = 0; i < p->nRestricoes; i++) {
        printf("%s %d %d\n", (p->restricoes + i)->nomeDisciplina,
                (p->restricoes + i)->dia, (p->restricoes + i)->periodo);
    }

}

void imprimeResposta(Problema *p, Individuo *ind) {
    int *t;
    int i, dia, periodo, sala;

    // melhor timetabling
    t = ind->aula;

    for (i = 0; i < p->dimensao; i++) {

        if (t[i] <= p->nAulas) { // nao eh horario vago
            dia = getDiaFromPos(i, p);
            periodo = getPeriodoFromPos(i, p);
            sala = getRoomFromPos(i, p);

            printf("%s %s %d %d\n", getDisciplina(p, t[i]), (p->salas + sala)->idSala, dia, periodo);
        }
    }
}
